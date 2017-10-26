
#include <robotican_hardware_interface/armadillo2_hardware_interface.h>

//#define DEBUG_PRINTS

class Armadillo2Robot : public hardware_interface::RobotHW
{
public:

    robotican_hardware::RiCBoardManager _boardManager;

    Armadillo2Robot(ros::NodeHandle &nh)
    {


_boardManager.connect();

while (ros::ok() && _boardManager.getConnectState() != ConnectEnum::Connected) { ros::Rate(10).sleep(); }
       if(ros::ok()) {
_boardManager.buildDevices();
_boardManager.buildDevices(&jnt_state_interface,&jnt_pos_interface);
_boardManager.buildDevices(&jnt_state_interface,&jnt_vel_interface);
       }

        initSpecFile();

        nh.param<std::string>("device", device_name, "/dev/USB2DYNAMIXEL");
        nh.param<int>("device_baudrate", device_baudrate, 1000000);

        if(nh.hasParam("dynamixel_motors")) {
            XmlRpc::XmlRpcValue dynamixel_motors;
            nh.getParam("dynamixel_motors", dynamixel_motors);
            if(!dynamixel_motors.getType() == XmlRpc::XmlRpcValue::TypeArray) {
                ROS_ERROR("[%s]: Invalid/missing servo information on the param server", ros::this_node::getName().c_str());
               exit(-1);
            }
            else {

                NUM_ARM_MOTORS = dynamixel_motors.size();
                for(int i = 0; i < NUM_ARM_MOTORS && ros::ok(); ++i) {



                    if(!dynamixel_motors[i].getType() == XmlRpc::XmlRpcValue::TypeStruct) {
                        ROS_ERROR("[%s]: Invalid/Missing info-struct for servo index %d",ros::this_node::getName().c_str() ,i);
                         exit(-1);
                    }
                    else {

                        struct motor m;
                        m.model = 0;
                        m.protocol_ver = 2.0;
                        m.command_position = 0.0;
                        m.command_velocity = 1.5;
                        m.pre_command_velocity=0.01;
m.first_read=false;
                        m.torque=false;

                        if(!dynamixel_motors[i]["id"].getType() == XmlRpc::XmlRpcValue::TypeInt) {
                            ROS_ERROR("[%s]: Invalid/Missing id for servo index %d", ros::this_node::getName().c_str(), i);
                             exit(-1);
                        }
                        else {
                            m.id = static_cast<int>(dynamixel_motors[i]["id"]);
                        }
                        if (!dynamixel_motors[i]["joint_name"].getType() == XmlRpc::XmlRpcValue::TypeString)
                        {
                            ROS_ERROR("[%s]: Invalid/Missing joint name for servo index %d, id: %d", ros::this_node::getName().c_str() ,i, m.id);
                             exit(-1);
                        }
                        else {
                           m.joint_name = static_cast<std::string>(dynamixel_motors[i]["joint_name"]);
                        }
                        if (!dynamixel_motors[i]["interface"].getType() == XmlRpc::XmlRpcValue::TypeString)
                        {
                            ROS_ERROR("[%s]: Invalid/Missing interface for servo index %d, id: %d", ros::this_node::getName().c_str() ,i, m.id);
                            exit(-1);
                        }
                        else {
                            m.interface = static_cast<std::string>(dynamixel_motors[i]["interface"]);
                        }
  if (!dynamixel_motors[i]["protocol"].getType() == XmlRpc::XmlRpcValue::TypeInt)
                        {
                            ROS_ERROR("[%s]: Invalid/Missing protocol for servo index %d, id: %d", ros::this_node::getName().c_str() ,i, m.id);
                            exit(-1);
                        }
                        else {
                            m.protocol_ver = static_cast<int>(dynamixel_motors[i]["protocol"]);
                        }

                        motors.push_back(m);
                    }

                }

            }
        }


        first=true;



        // connect and register the joint state interface
        for (int i=0;i<NUM_ARM_MOTORS;i++){

            state_handles.push_back( hardware_interface::JointStateHandle(motors[i].joint_name, &motors[i].position, &motors[i].velocity, &motors[i].effort));
            jnt_state_interface.registerHandle(state_handles[i]);
        }

        //pan tilt torso
      //  state_handles.push_back( hardware_interface::JointStateHandle("head_pan_joint", &pan_position, &pan_velocity, &pan_effort));
     //   jnt_state_interface.registerHandle(state_handles[state_handles.size()-1]);
      //  state_handles.push_back( hardware_interface::JointStateHandle("head_tilt_joint", &tilt_position, &tilt_velocity, &tilt_effort));
     //   jnt_state_interface.registerHandle(state_handles[state_handles.size()-1]);
       // state_handles.push_back( hardware_interface::JointStateHandle("torso_joint", &torso_position, &torso_velocity, &torso_effort));
        //jnt_state_interface.registerHandle(state_handles[state_handles.size()-1]);

        //wheels

       // state_handles.push_back( hardware_interface::JointStateHandle("left_wheel_joint", &left_wheel_position, &left_wheel_velocity, &left_wheel_effort));
       // jnt_state_interface.registerHandle(state_handles[state_handles.size()-1]);
        //state_handles.push_back( hardware_interface::JointStateHandle("right_wheel_joint", &right_wheel_position, &right_wheel_velocity, &right_wheel_effort));
       // jnt_state_interface.registerHandle(state_handles[state_handles.size()-1]);


        registerInterface(&jnt_state_interface);


        for (int i=0;i<NUM_ARM_MOTORS;i++){
            if (motors[i].interface=="PosVel") {
                posvel_handles.push_back( hardware_interface::PosVelJointHandle(jnt_state_interface.getHandle(motors[i].joint_name), &motors[i].command_position, &motors[i].command_velocity));
                jnt_posvel_interface.registerHandle(posvel_handles[posvel_handles.size()-1]);
             //   ROS_WARN("posvel     id: %d      %s",motors[i].id,motors[i].joint_name.c_str());
            }
            else if (motors[i].interface=="Pos") {

                pos_handles.push_back( hardware_interface::JointHandle(jnt_state_interface.getHandle(motors[i].joint_name), &motors[i].command_position));
             //   ROS_WARN("pos     id: %d      %s",motors[i].id,motors[i].joint_name.c_str());
                jnt_pos_interface.registerHandle(pos_handles[pos_handles.size()-1]);

            }
        }

        //pan tilt torso
   //     pos_handles.push_back( hardware_interface::JointHandle(jnt_state_interface.getHandle("head_pan_joint"), &pan_command_position));
// jnt_pos_interface.registerHandle(pos_handles[pos_handles.size()-1]);
//pos_handles.push_back( hardware_interface::JointHandle(jnt_state_interface.getHandle("head_tilt_joint"), &tilt_command_position));
//jnt_pos_interface.registerHandle(pos_handles[pos_handles.size()-1]);
//pos_handles.push_back( hardware_interface::JointHandle(jnt_state_interface.getHandle("torso_joint"), &torso_command_position));
//jnt_pos_interface.registerHandle(pos_handles[pos_handles.size()-1]);


//wheels
//vel_handles.push_back( hardware_interface::JointHandle(jnt_state_interface.getHandle("left_wheel_joint"), &left_wheel_command_velocity));
//jnt_vel_interface.registerHandle(vel_handles[vel_handles.size()-1]);
//vel_handles.push_back( hardware_interface::JointHandle(jnt_state_interface.getHandle("right_wheel_joint"), &right_wheel_command_velocity));
//jnt_vel_interface.registerHandle(vel_handles[vel_handles.size()-1]);


        registerInterface(&jnt_posvel_interface);
        registerInterface(&jnt_pos_interface);
        registerInterface(&jnt_vel_interface);


        //BUILD DYNAMIXEL MOTORS

        packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION2);
// packetHandler1 = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION1);

        portHandler = dynamixel::PortHandler::getPortHandler(device_name.c_str());

        // Open port
        if (portHandler->openPort())
        {
            portHandler->setBaudRate(device_baudrate);
            ROS_INFO("Succeeded to open the port!");
            ROS_INFO(" - Device Name : %s", device_name.c_str());
            ROS_INFO(" - Baudrate    : %d", portHandler->getBaudRate());
        }
        else
        {
            ROS_ERROR("Failed to open the port! [%s]\n", device_name.c_str());
            ros::shutdown();
        }


        ROS_INFO("Pinging motors...");
        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            int err_cnt=0;
            while ((!ping(motors[i]) || motors[i].model == 0)&&ros::ok()) {
                ROS_ERROR("[ID:%03d] ping error",motors[i].id);
                ros::Rate(5).sleep();
                if(err_cnt++==5) {
                    ROS_ERROR("[ID:%03d] too many ping errors, shuting down",motors[i].id);
                    ros::shutdown();
                    exit(-1);
                }
            }
#ifdef DEBUG_PRINTS
            printf("[ID:%03d] Ping OK, model: %d\n", motors[i].id, motors[i].model);
#endif

            bool success = _modelSpec.find(motors[i].model) != _modelSpec.end();
            if (success) {
                motors[i].cpr=_modelSpec[motors[i].model].cpr;
                motors[i].rpm_scale_factor=_modelSpec[motors[i].model].rpm_scale_factor;
                motors[i].torque_constant_a=_modelSpec[motors[i].model].torque_constant_a;
                motors[i].torque_constant_b=_modelSpec[motors[i].model].torque_constant_b;
            }

        }
        if (!ros::ok()) ros::shutdown();

        ROS_INFO("Enabling motors torque...");
        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            int err_cnt=0;
            while (!setTorque(motors[i], true) && ros::ok()) {
                ROS_ERROR("[ID:%03d] setTorque error",motors[i].id);
                ros::Rate(5).sleep();
                if(err_cnt++==5) {
                    ROS_ERROR("[ID:%03d] too many setTorque errors, shuting down",motors[i].id);
                    ros::shutdown();
exit(-1);
                }
            }
#ifdef DEBUG_PRINTS
            printf("[ID:%03d] Torque ON\n", i);
#endif
            motors[i].torque = true;
        }

        if (!ros::ok()) ros::shutdown();

        ROS_INFO("Armadillo2 Hardware Interface loop launched successfully");

        _time = ros::Time::now();

    }

    // DYNAMIXEL MOTORS FUNCTIONS
    double ticks2rads(int32_t ticks, struct motor &motor) {
        if (motor.protocol_ver == 2.0) {

            if (motor.model==1040) {
                const double FromTicks = 1.0 / (motor.cpr / 2.0);
                return static_cast<double>(M_PI-(ticks) * FromTicks * M_PI);
            }
 else if (motor.model==30) {
            double cprDev2 = motor.cpr / 2.0f;
            return (static_cast<double>(ticks) - cprDev2) * M_PI / cprDev2;
        }
            else {
            const double FromTicks = 1.0 / (motor.cpr / 2.0);
            return static_cast<double>((ticks) * FromTicks * M_PI);
            }
        }
        else {
            double cprDev2 = motor.cpr / 2.0f;
            return (static_cast<double>(ticks) - cprDev2) * M_PI / cprDev2;
        }
    }

    int32_t rads2ticks(double rads, struct motor &motor) {

        if (motor.protocol_ver == 2.0) {
            if (motor.model==1040) {
                return static_cast<int32_t>(round((-rads *180.0/ M_PI+180.0)/ 0.088));
            }
 else if (motor.model==30) {
               double cprDev2 = motor.cpr / 2.0f;
            return static_cast<int32_t>(round(cprDev2 + (rads * cprDev2 / M_PI)));
            }
            else {
            double cprDev2 = motor.cpr / 2.0f;
            return static_cast<int32_t>(round((rads / M_PI) * cprDev2));
            }
        }
        else {
            double cprDev2 = motor.cpr / 2.0f;
            return static_cast<int32_t>(round(cprDev2 + (rads * cprDev2 / M_PI)));
        }
    }

    int32_t rad_s2ticks_s(double rads, struct motor &motor)  {
//if (motor.id==7) printf("[7] rad/s: %f\n",rads);
        if (motor.protocol_ver == 2.0)       
            return static_cast<int32_t >(rads / 2.0 / M_PI * 60.0 / motor.rpm_scale_factor);

        else {

            return static_cast<int32_t >(83.49f * (rads)-0.564f);//TODO: Verify
        }

    }

    double ticks_s2rad_s(int32_t ticks, struct motor &motor)  {
        if (motor.protocol_ver == 2.0) {
            return ((double)ticks) * 2.0 * M_PI / 60.0 / motor.rpm_scale_factor;
        }
        else {
            return (100.0f / 8349.0f) * ((double)ticks) + (94.0f / 13915.0f);
        }
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
            //  printf("[ID:%03d] ping Succeeded.  model: %d\n", motor.id,motor.model);
            return true;
        }

    }


    bool setTorque(struct motor &motor, bool onoff)
    {

// if (motor.model == 30) return true;
        uint8_t dxl_error = 0;
        int dxl_comm_result = COMM_TX_FAIL;
        uint16_t addr = ADDR_PRO_TORQUE_ENABLE;
        if (motor.model == 1040) addr = ADDR_XH_TORQUE_ENABLE;
       else  if (motor.model == 30) addr = ADDR_MX_TORQUE_ENABLE;

//printf("addr: %d\n",addr);
 dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, motor.id, addr, onoff, &dxl_error);


        if (dxl_comm_result != COMM_SUCCESS) {
            //packetHandler->printTxRxResult(dxl_comm_result);
            return false;
        }
        else if (dxl_error != 0) {
            // packetHandler->printRxPacketError(dxl_error);
            return false;
        }
        else {
            return true;
        }
    }

    bool bulk_write() {


        int8_t dxl_comm_result_ = 0;


        dynamixel::GroupBulkWrite GroupBulkWrite(portHandler, packetHandler);


        for (int i = 0; i < NUM_ARM_MOTORS; i++) {
if (motors[i].first_read==false) continue;
            bool dxl_addparam_result = false;
            uint16_t addr = ADDR_PRO_GOAL_SPEED;
            if (motors[i].model == 1040) addr = ADDR_XH_PROFILE_VELOCITY;
else if (motors[i].model == 30) addr = ADDR_MX_PROFILE_VELOCITY;
            if (motors[i].command_velocity==0) {
             //   if (motors[i].id==2) ROS_ERROR("command_velocity==0     pre_rad_s: %f",pre_rad_s);
                motors[i].command_velocity= motors[i].pre_command_velocity;

            }

            motors[i].pre_command_velocity=motors[i].command_velocity;
            int32_t vel = rad_s2ticks_s(motors[i].command_velocity, motors[i]);
            //  if (motors[i].id==2) ROS_INFO("[2] command_velocity: %f     vel: %d",motors[i].command_velocity,(int)vel);
            dxl_addparam_result = GroupBulkWrite.addParam(motors[i].id, addr, 4, (uint8_t*)&vel);
            if (dxl_addparam_result != true)
            {
                printf("[ID:%03d] 2GroupBulkWrite addparam failed", motors[i].id);
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


        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
if (motors[i].first_read==false) continue;
            bool dxl_addparam_result = false;                // addParam result
            uint16_t addr = ADDR_PRO_GOAL_POSITION;
            if (motors[i].model == 1040) addr = ADDR_XH_GOAL_POSITION;
            else if (motors[i].model == 30) addr = ADDR_MX_GOAL_POSITION;
            int32_t pos = rads2ticks(motors[i].command_position, motors[i]);
            dxl_addparam_result = GroupBulkWrite.addParam(motors[i].id, addr, 4, (uint8_t*)&pos);
            if (dxl_addparam_result != true)
            {
                printf("[ID:%03d] 1GroupBulkWrite addparam failed", motors[i].id);
                return false;
            }
           // if (motors[i].id==7||motors[i].id==8) {
            //     printf("[ID:%03d] write pos %f\n", motors[i].id,motors[i].command_position);
            //}
            //_sleep(100);

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

    bool bulk_read_err() {


        dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_addparam_result = false;                // addParam result
            uint16_t addr = ADDR_PRO_HARDWARE_ERROR;
            if (motors[i].model == 1040) addr = ADDR_XH_HARDWARE_ERROR;
 else if (motors[i].model == 30) addr = ADDR_MX_HARDWARE_ERROR;
            dxl_addparam_result = GroupBulkRead.addParam(motors[i].id, addr, 1); //
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

        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_getdata_result = false; // GetParam result
            // Check if groupsyncread data of Dynamixels are available
            uint16_t addr = ADDR_PRO_HARDWARE_ERROR;
            if (motors[i].model == 1040) addr = ADDR_XH_HARDWARE_ERROR;
 else if (motors[i].model == 30) addr = ADDR_MX_HARDWARE_ERROR;
            dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 1);
            if (dxl_getdata_result != true)
            {
                // ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
                return false;
            }
            else {
                uint16_t addr = ADDR_PRO_HARDWARE_ERROR;
                if (motors[i].model == 1040) addr = ADDR_XH_HARDWARE_ERROR;
 else if (motors[i].model == 30) addr = ADDR_MX_HARDWARE_ERROR;
                motors[i].error = GroupBulkRead.getData(motors[i].id, addr, 1);
                if (motors[i].error!=0) ROS_ERROR("[ID:%03d] HARDWARE ERROR: %d",motors[i].id,motors[i].error);

            }
        }
//puts("--err ok");
        return true;
    }

    bool bulk_read_pos() {

//puts("--pos start");
        dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_addparam_result = false;                // addParam result
            uint16_t addr = ADDR_PRO_PRESENT_POSITION;
            if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_POSITION;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_POSITION;
            dxl_addparam_result = GroupBulkRead.addParam(motors[i].id, addr, 4); //
            if (dxl_addparam_result != true)
            {
               // ROS_ERROR("[ID:%03d] groupSyncRead addparam failed", motors[i].id);
                return false;
            }
        }

        int dxl_comm_result = COMM_TX_FAIL;
        dxl_comm_result = GroupBulkRead.txRxPacket();
        if (dxl_comm_result != COMM_SUCCESS)
        {
            // ROS_ERROR("txRxPacket Error");
            packetHandler->printTxRxResult(dxl_comm_result);
            return false;
        }

        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_getdata_result = false; // GetParam result
            // Check if groupsyncread data of Dynamixels are available
            uint16_t addr = ADDR_PRO_PRESENT_POSITION;
            if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_POSITION;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_POSITION;
            dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 4);
            if (dxl_getdata_result != true)
            {
               //  ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", motors[i].id);
                return false;
            }
            else {
                uint16_t addr = ADDR_PRO_PRESENT_POSITION;
                if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_POSITION;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_POSITION;
                motors[i].position = ticks2rads(GroupBulkRead.getData(motors[i].id, addr, 4), motors[i]);
if (motors[i].first_read==false) {
motors[i].first_read=true;
 motors[i].command_position = motors[i].position;

if (motors[i].joint_name == "head_pan_joint" || motors[i].joint_name == "head_tilt_joint") {
 motors[i].command_position=0;
ROS_WARN("Centering  %s",motors[i].joint_name.c_str());
}
}
         //        ROS_INFO("[ID:%03d] Read bulk pos value: %f",motors[i].id,motors[i].position);

            }
        }
//puts("--pos ok");
        return true;
    }

    bool bulk_read_vel() {


        dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_addparam_result = false;                // addParam result
            uint16_t addr = ADDR_PRO_PRESENT_SPEED;
            if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_SPEED;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_SPEED;
            dxl_addparam_result = GroupBulkRead.addParam(motors[i].id, addr, 4); //
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

        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_getdata_result = false; // GetParam result
            // Check if groupsyncread data of Dynamixels are available
            uint16_t addr = ADDR_PRO_PRESENT_SPEED;
            if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_SPEED;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_SPEED;
            dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 4);
            if (dxl_getdata_result != true)
            {
                // ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
                return false;
            }
            else {
                uint16_t addr = ADDR_PRO_PRESENT_SPEED;
                if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_SPEED;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_SPEED;
                motors[i].velocity = ticks_s2rad_s(GroupBulkRead.getData(motors[i].id, addr, 4), motors[i]);
//ROS_INFO("[ID:%03d] Read bulk vel value: %f",motors[i].id,motors[i].velocity);
                // ROS_INFO("[ID:%03d] Read sync pos value: %d",i,value);

            }
        }
//puts("--vel ok");
        return true;
    }

    bool bulk_read_load() {


        dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_addparam_result = false;                // addParam result
            uint16_t addr = ADDR_PRO_PRESENT_CURRENT;
            if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_CURRENT;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_LOAD;
            dxl_addparam_result = GroupBulkRead.addParam(motors[i].id, addr, 2); //
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

        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            bool dxl_getdata_result = false; // GetParam result
            // Check if groupsyncread data of Dynamixels are available
            uint16_t addr = ADDR_PRO_PRESENT_CURRENT;
            if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_CURRENT;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_LOAD;
            dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 2);
            if (dxl_getdata_result != true)
            {
                // ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
                return false;
            }
            else {
                uint16_t addr = ADDR_PRO_PRESENT_CURRENT;
                if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_CURRENT;
else if (motors[i].model == 30) addr = ADDR_MX_PRESENT_LOAD;
                motors[i].current = (int16_t)GroupBulkRead.getData(motors[i].id, addr, 2) ;
                if (motors[i].model == 53768 || motors[i].model == 54024) motors[i].current *= 33000.0 / 2048.0 / 1000.0;
                else if (motors[i].model == 51200) motors[i].current *= 8250.0 / 2048.0 / 1000.0;
                else if (motors[i].model == 1040) motors[i].current *= 2.69/1000.0;
else if (motors[i].model == 30) motors[i].current *= 0.1; //percentage -100 ~ 100 %

                motors[i].effort=motors[i].torque_constant_a*motors[i].current+motors[i].torque_constant_b;
                // ROS_INFO("[ID:%03d] Read sync pos value: %d",i,value);33,000 /  2,048
//ROS_INFO("[ID:%03d] Read bulk load value: %f",motors[i].id,motors[i].effort);

            }
        }
//puts("--load ok");
        return true;
    }

    ~Armadillo2Robot() {
        portHandler->closePort();
         _boardManager.disconnect();
        //  free(motors);
    }

    void read() {
        bool read_ok = bulk_read_pos() && bulk_read_vel() && bulk_read_load() && bulk_read_err();
        if (read_ok) {
#ifdef DEBUG_PRINTS
            //ROS_INFO("Read OK");
            printf("POS: ");
            for (int i = 0; i<NUM_ARM_MOTORS; i++) {
                printf("%.3f, ", motors[i].position);
            }
            puts("");
            printf("VEL: ");
            for (int i = 0; i<NUM_ARM_MOTORS; i++) {
                printf("%.3f, ", motors[i].velocity);
            }
            puts("");
            printf("EFF: ");
            for (int i = 0; i<NUM_ARM_MOTORS; i++) {
                printf("%.3f, ", motors[i].effort);
            }
            puts("");
            printf("ERR: ");
            for (int i = 0; i<NUM_ARM_MOTORS; i++) {
                printf("%d, ", motors[i].error);
            }
            puts("");
#endif
        }
        else {

            ROS_ERROR("Read error");

           // if (first) return;
        }



       // pan_position=pan_command_position;
       // tilt_position=tilt_command_position;
        //right_wheel_velocity=right_wheel_command_velocity;
        //left_wheel_velocity=left_wheel_command_velocity;


    }

    void write() {

         _boardManager.write();
        /* static float s = 1;
        for (int i = 0; i < NUM_ARM_MOTORS; i++) {
            motors[i].command_position+= 0.0005*s;
            if (motors[i].command_position > 0.1) s = -1;
            else if (motors[i].command_position < -0.1) s = 1;
        }
        */
#ifdef DEBUG_PRINTS
        //  printf("command= %.3f\n", motors[0].command_position);
        printf("DIF: ");
        for (int i = 0; i<NUM_ARM_MOTORS; i++) {
            printf("%.3f, ", motors[i].command_position-motors[i].position);
        }
        puts("");
#endif

        bool write_ok = bulk_write();
        if (write_ok) {
            //ROS_INFO("Write OK");
        }
        else ROS_ERROR("Write error");

    }
    ros::Time getTime() {
        return ros::Time::now();
    }

    ros::Duration getPeriod() {
        ros::Time now = ros::Time::now();
        ros::Duration period = now - _time;
        _time = now;
        return period;
    }


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
            doc[i]["rpm_scale_factor"]  >> spec.rpm_scale_factor;
            doc[i]["torque_constant_a"]  >> spec.torque_constant_a;
            doc[i]["torque_constant_b"]  >> spec.torque_constant_b;


            _modelSpec[spec.model_number] = spec;
        }

    }


private:





    bool first;
    ros::Time _time;

    hardware_interface::JointStateInterface jnt_state_interface;
    hardware_interface::PosVelJointInterface jnt_posvel_interface;
    hardware_interface::PositionJointInterface jnt_pos_interface;
    hardware_interface::VelocityJointInterface jnt_vel_interface;

    dynamixel::PacketHandler *packetHandler;
 //dynamixel::PacketHandler *packetHandler1;
    dynamixel::PortHandler *portHandler;
    std::vector<struct motor> motors;
    std::vector<hardware_interface::JointStateHandle> state_handles;
    std::vector<hardware_interface::PosVelJointHandle> posvel_handles;
    std::vector<hardware_interface::JointHandle> pos_handles,vel_handles;


    std::map<uint16_t, dynamixel_spec> _modelSpec;
int NUM_ARM_MOTORS;

std::string device_name;
int device_baudrate;


//double pan_position,pan_velocity,pan_effort,pan_command_position;
//double tilt_position,tilt_velocity,tilt_effort,tilt_command_position;

};

main(int argc, char **argv)
{
    ros::init(argc, argv, "armadillo2_hardware_interface_node");
    ros::NodeHandle nodeHandle;
    Armadillo2Robot robot(nodeHandle);
    controller_manager::ControllerManager cm(&robot);
    ros::AsyncSpinner asyncSpinner(2);
    asyncSpinner.start();
    ros::Rate loop(100);
    while (ros::ok()) {
        robot.read();
        cm.update(robot.getTime(), robot.getPeriod());
loop.sleep();
        robot.write();
        loop.sleep();
    }
}
