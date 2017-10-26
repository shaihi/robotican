

#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdio.h>

#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <dynamixel_sdk/dynamixel_sdk.h>
#include "robotican_hardware_interface/dynamixel_driver.h"
#include <ros/package.h>
#include "yaml-cpp/yaml.h"


double ticks2rads(int32_t ticks, struct motor &motor);
int32_t rads2ticks(double rads, struct motor &motor);

dynamixel::PortHandler *portHandler;
dynamixel::PacketHandler *packetHandler;

#ifdef HAVE_NEW_YAMLCPP
// The >> operator disappeared in yaml-cpp 0.5, so this function is
// added to provide support for code written under the yaml-cpp 0.3 API.
template<typename T>
void operator >> (const YAML::Node& node, T& i)
{
    i = node.as<T>();
}

#endif

// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting

#define BAUDRATE                        57600
#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller
// ex) Windows: "COM1"   Linux: "/dev/ttyUSB0" Mac: "/dev/tty.usbserial-*"
struct motor
{
    uint8_t id;
    uint16_t model;
    bool torque;
    double position;
    double command_position;
    double command_velocity;
    int cpr;
    double gear_reduction;
    float protocol_ver;
    std::string joint_name;
};
struct motor *motors;
int num_motors=0;

struct dynamixel_spec
{
    std::string name;
    uint16_t model_number;
    int cpr;
    double gear_reduction;
};
std::map<uint16_t, dynamixel_spec> _modelSpec;

void initSpecFile() {
    std::string path = ros::package::getPath("robotican_hardware_interface");
    path += "/config/motor_data.yaml";
    YAML::Node doc;

#ifdef HAVE_NEW_YAMLCPP
    doc = YAML::LoadFile(path);
#else
    ifstream fin(path.c_str());
    YAML::Parser parser(fin);
    parser.GetNextDocument(doc);
#endif
    for (int i = 0; i < doc.size(); i++)
    {
        dynamixel_spec spec;

        // Load the basic specs of this motor type
        doc[i]["name"] >> spec.name;
        doc[i]["model_number"] >> spec.model_number;
        doc[i]["cpr"]  >> spec.cpr;
        doc[i]["gear_reduction"]  >> spec.gear_reduction;

        _modelSpec[spec.model_number] = spec;
    }

}

int getch()
{
#if defined(__linux__) || defined(__APPLE__)
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#elif defined(_WIN32) || defined(_WIN64)
    return _getch();
#endif
}

int kbhit(void)
{
#if defined(__linux__) || defined(__APPLE__)
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
#elif defined(_WIN32) || defined(_WIN64)
    return _kbhit();
#endif
}


bool setTorque(struct motor &motor, bool onoff)
{
    uint8_t dxl_error = 0;
    int dxl_comm_result = COMM_TX_FAIL;
    uint16_t addr=ADDR_PRO_TORQUE_ENABLE;
    if (motor.model==1040) addr=ADDR_XH_TORQUE_ENABLE;

    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, motor.id, addr, onoff, &dxl_error);

    if(dxl_comm_result != COMM_SUCCESS) {
        //packetHandler->printTxRxResult(dxl_comm_result);
        return false;
    }
    else if(dxl_error != 0) {
        // packetHandler->printRxPacketError(dxl_error);
        return false;
    }
    else {
        return true;
    }
}

bool bulk_write() {


    int8_t dxl_comm_result_=0;



    dynamixel::GroupBulkWrite GroupBulkWrite(portHandler, packetHandler);
    for (int i=0;i<num_motors;i++) {
        bool dxl_addparam_result = false;                // addParam result
        uint16_t addr=ADDR_PRO_GOAL_POSITION;
        if (motors[i].model==1040) addr=ADDR_XH_GOAL_POSITION;
        int32_t pos=rads2ticks(motors[i].command_position,motors[i]);
        dxl_addparam_result =GroupBulkWrite.addParam(motors[i].id,addr,4,(uint8_t*)&pos);

        if (dxl_addparam_result != true)
        {
            //  ROS_ERROR("[ID:%03d] groupSyncRead addparam failed", i);
            return false;

        }
    }

    dxl_comm_result_ = GroupBulkWrite.txPacket();

    if (dxl_comm_result_ != COMM_SUCCESS)
    {
        // ROS_ERROR("txPacket Error");
        // packetHandler->printTxRxResult(dxl_comm_result_);
        return false;
    }

    GroupBulkWrite.clearParam();
    return true;

}

bool bulk_read() {


    dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
    for (int i=0;i<num_motors;i++) {
        bool dxl_addparam_result = false;                // addParam result
        uint16_t addr=ADDR_PRO_PRESENT_POSITION;
        if (motors[i].model==1040) addr=ADDR_XH_PRESENT_POSITION;

        dxl_addparam_result =GroupBulkRead.addParam(motors[i].id,addr, 4); //

        if (dxl_addparam_result != true)
        {
            //ROS_ERROR("[ID:%03d] groupSyncRead addparam failed", i);
            return false;

        }
    }

    int dxl_comm_result = COMM_TX_FAIL;
    dxl_comm_result = GroupBulkRead.txRxPacket();
    if (dxl_comm_result != COMM_SUCCESS)
    {
        // ROS_ERROR("txRxPacket Error");
        //  packetHandler->printTxRxResult(dxl_comm_result);
        return false;
    }

    for (int i=0;i<num_motors;i++) {
        bool dxl_getdata_result = false; // GetParam result
        // Check if groupsyncread data of Dynamixels are available
        uint16_t addr=ADDR_PRO_PRESENT_POSITION;
        if (motors[i].model==1040) addr=ADDR_XH_PRESENT_POSITION;
        dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 4);
        if (dxl_getdata_result != true)
        {
            // ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
            return false;
        }
        else {
            uint16_t addr=ADDR_PRO_PRESENT_POSITION;
            if (motors[i].model==1040) addr=ADDR_XH_PRESENT_POSITION;
            motors[i].position  = ticks2rads(GroupBulkRead.getData(motors[i].id,  addr, 4),motors[i]);
            // ROS_INFO("[ID:%03d] Read sync pos value: %d",i,value);

        }
    }
    return true;
}

bool ping(struct motor &motor) {

    int dxl_comm_result = COMM_TX_FAIL;             // Communication result
    uint8_t dxl_error = 0;                          // Dynamixel error



    dxl_comm_result = packetHandler->ping(portHandler, motor.id, &(motor.model), &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
        // ROS_ERROR("[ID:%03d] Ping TXRX error: ",id);
        //  packetHandler->printTxRxResult(dxl_comm_result);
        return false;
    }
    else if (dxl_error != 0)
    {
        // ROS_ERROR("[ID:%03d] Ping error: ",id);
        //  packetHandler->printRxPacketError(dxl_error);
        return false;

    }
    else {
        //  printf("[ID:%03d] ping Succeeded.\n", motors[id].id);
        return true;
    }

}

double ticks2rads(int32_t ticks, struct motor &motor) {
    if(motor.protocol_ver == PROTOCOL2_VERSION) {
        const double FromTicks = 1.0 / (motor.cpr / 2.0);
        return static_cast<double>(ticks) * FromTicks * M_PI;
    }
    else {
        double cprDev2 = motor.cpr / 2.0f;
        return (static_cast<double>(ticks) - cprDev2) * M_PI / cprDev2;
    }
}

int32_t rads2ticks(double rads, struct motor &motor) {

    if(motor.protocol_ver == PROTOCOL2_VERSION) {
        double cprDev2 = motor.cpr / 2.0f;
        return static_cast<int32_t>(round((rads / M_PI) * cprDev2));
    }
    else {
        double cprDev2 = motor.cpr / 2.0f;
        return static_cast<int32_t>(round(cprDev2 + (rads * cprDev2 / M_PI)));
    }
}


void CmdCallback(const sensor_msgs::JointStateConstPtr &msg) {
    int size =msg->position.size();
    for(int i = 0; i < size; ++i) {
        motors[i].command_position=msg->position[i];
        motors[i].command_velocity=msg->velocity[i];
    }

    bulk_write();
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "ping");
    ros::NodeHandle nh;
    //ros::AsyncSpinner spinner(2);
    //spinner.start();

    ros::Subscriber cmdListener=nh.subscribe("dxl_command", 10, &CmdCallback);
    initSpecFile();

    portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);
    packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

    // Open port
    if (portHandler->openPort()) printf("Succeeded to open the port!\n");
    else {
        printf("Failed to open the port!\n");
        return 0;
    }
    // Set port baudrate
    if (portHandler->setBaudRate(BAUDRATE)) printf("Succeeded to change the baudrate!\n");
    else
    {
        printf("Failed to change the baudrate!\n");
        return 0;
    }
    // Set timeout
    //portHandler->setPacketTimeout(111111111.0);


    uint8_t ids[]={1,2,3,4,5,6,7,8};

    num_motors=sizeof(ids)/sizeof(uint8_t);
    motors=(struct motor *)malloc(sizeof(struct motor)*num_motors);
    for (int i=0;i<num_motors;i++) {
        motors[i].id=ids[i];
        motors[i].model=0;
        motors[i].protocol_ver=2.0;
        motors[i].command_position=0;
        motors[i].command_velocity=rads2ticks(0.5,motors[i]);
    }

    ROS_INFO("Pinging motors...");
    for (int i=0;i<num_motors;i++) {
        while ((!ping(motors[i])|| motors[i].model==0)&&ros::ok()) {
            //ROS_WARN("[ID:%03d] ping error",motors[i].id);
            ros::Rate(5).sleep();
            // ros::spinOnce();
        }
        ROS_INFO("[ID:%03d] Ping OK, model: %d",motors[i].id,motors[i].model);
        bool success = _modelSpec.find(motors[i].model) != _modelSpec.end();
        if (success) {
            motors[i].cpr=_modelSpec[motors[i].model].cpr;
            motors[i].gear_reduction=_modelSpec[motors[i].model].gear_reduction;
        }
    }

    ROS_INFO("Enabling motors torque...");
    for (int i=0;i<num_motors;i++) {
        while (!setTorque(motors[i],false)&&ros::ok()) {

            // ROS_WARN("[ID:%03d] setTorque error",i);
            ros::Rate(5).sleep();
        }
        ROS_INFO("[ID:%03d] Torque ON",i);
        motors[i].torque=true;
    }
    ROS_INFO("Read Write loop started");
    while (ros::ok()) {


        bool read_ok=bulk_read();
        if (read_ok) {
            //ROS_INFO("Read OK");
            for (int i=0;i<num_motors;i++) {
                printf("%.3f, ",motors[i].position);
            }
            puts("");
        }
        else {
            //  ROS_ERROR("Read error");
        }
        //ros::Rate(200).sleep();

        bool write_ok=1;//bulk_write(portHandler,packetHandler);
        if (write_ok) {
            //ROS_INFO("Write OK");
        }
        else ROS_ERROR("Write error");


        ros::Rate(200).sleep();
        ros::spinOnce();
    }

    // Close port
    portHandler->closePort();
    free(motors);
    return 0;
}
