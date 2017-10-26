

#ifdef __linux__
#include "ros/ros.h"
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#define _USE_MATH_DEFINES // for C++  

#include <cmath>  
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>


#include "dynamixel_sdk/dynamixel_sdk.h"                                  // Uses Dynamixel SDK library


// Control table address FOR MX-28
#define ADDR_MX_MODEL_NUM               0
#define ADDR_MX_TORQUE_ENABLE           24                  // Control table address is different in Dynamixel model
#define ADDR_MX_GOAL_POSITION           30
#define ADDR_MX_GOAL_SPEED              32
#define ADDR_MX_PRESENT_POSITION        36
#define ADDR_MX_PRESENT_SPEED           38
#define ADDR_MX_PRESENT_LOAD            40
#define ADDR_MX_PRESENT_TEMPERATURE     43
#define ADDR_MX_MOVING                  46
#define ADDR_MX_GOAL_ACCELERATION       73

// Control table address FOR Pro
#define ADDR_PRO_MODEL_NUM               0
#define ADDR_PRO_TORQUE_ENABLE           562                  // Control table address is different in Dynamixel model
#define ADDR_PRO_GOAL_POSITION           596
#define ADDR_PRO_GOAL_SPEED              600           
#define ADDR_PRO_GOAL_ACCELERATION       606
#define ADDR_PRO_PRESENT_POSITION        611
#define ADDR_PRO_PRESENT_SPEED           615
#define ADDR_PRO_PRESENT_CURRENT         621
#define ADDR_PRO_PRESENT_TEMPERATURE     43
#define ADDR_PRO_MOVING                  46
#define ADDR_PRO_HARDWARE_ERROR          892


// Control table address FOR XH
#define ADDR_XH_MODEL_NUM               0
#define ADDR_XH_TORQUE_ENABLE           64                  // Control table address is different in Dynamixel model
#define ADDR_XH_GOAL_POSITION           116
#define ADDR_XH_GOAL_SPEED              104
#define ADDR_XH_GOAL_ACCELERATION       40
#define ADDR_XH_PRESENT_POSITION        132
#define ADDR_XH_PRESENT_SPEED           128
#define ADDR_XH_PRESENT_CURRENT         126
#define ADDR_XH_PRESENT_TEMPERATURE     146
#define ADDR_XH_MOVING                  123
#define ADDR_XH_HARDWARE_ERROR          70

// Protocol version
#define PROTOCOL_VERSION1               1.0                 // See which protocol version is used in the Dynamixel
#define PROTOCOL_VERSION2               2.0

// Default setting
#define DEVICENAME                      "/dev/USB2DYNAMIXEL"      // Check which port is being used on your controller
#define BAUDRATE                      1000000
                                                       // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"
struct motor
{
	uint8_t id;
	uint16_t model;
	bool torque;
	double position;
	double velocity;
	double current;
	double command_position;
	double command_velocity;
    uint8_t error;
	int cpr;
	double gear_reduction;
	float protocol_ver;
    std::string joint_name;
};
struct motor *motors;
int num_motors = 0;

dynamixel::PacketHandler *packetHandler;
dynamixel::PortHandler *portHandler;


int getch()
{
#ifdef __linux__
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
#ifdef __linux__
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


double ticks2rads(int32_t ticks, struct motor &motor) {
	if (motor.protocol_ver == 2.0) {
		const double FromTicks = 1.0 / (motor.cpr / 2.0);
		return static_cast<double>(ticks) * FromTicks * M_PI;
	}
	else {
		double cprDev2 = motor.cpr / 2.0f;
		return (static_cast<double>(ticks) - cprDev2) * M_PI / cprDev2;
	}
}

int32_t rads2ticks(double rads, struct motor &motor) {

	if (motor.protocol_ver == 2.0) {
		double cprDev2 = motor.cpr / 2.0f;
		return static_cast<int32_t>(round((rads / M_PI) * cprDev2));
	}
	else {
		double cprDev2 = motor.cpr / 2.0f;
		return static_cast<int32_t>(round(cprDev2 + (rads * cprDev2 / M_PI)));
	}
}

int32_t rad_s2ticks_s(double rads, struct motor &motor)  {
	if (motor.protocol_ver == 2.0)
		return static_cast<int32_t >(rads / 2.0 / M_PI * 60.0 * motor.gear_reduction);
	else {
		
		return static_cast<int32_t >(83.49f * (rads)-0.564f);
	}

}

double ticks_s2rad_s(int32_t ticks, struct motor &motor)  {
	if (motor.protocol_ver == 2.0) {
		return ((double)ticks) * 2.0 * M_PI / 60.0 / motor.gear_reduction;
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
		//  printf("[ID:%03d] ping Succeeded.\n", motors[id].id);
		return true;
	}

}


bool setTorque(struct motor &motor, bool onoff)
{
	uint8_t dxl_error = 0;
	int dxl_comm_result = COMM_TX_FAIL;
	uint16_t addr = ADDR_PRO_TORQUE_ENABLE;
	if (motor.model == 1040) addr = ADDR_XH_TORQUE_ENABLE;

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
	for (int i = 0; i<num_motors; i++) {
		bool dxl_addparam_result = false;                // addParam result
		uint16_t addr = ADDR_PRO_GOAL_POSITION;
		if (motors[i].model == 1040) addr = ADDR_XH_GOAL_POSITION;
		int32_t pos = rads2ticks(motors[i].command_position, motors[i]);
		dxl_addparam_result = GroupBulkWrite.addParam(motors[i].id, addr, 4, (uint8_t*)&pos);
		if (dxl_addparam_result != true)
		{
			printf("[ID:%03d] 1GroupBulkWrite addparam failed", motors[i].id);
			return false;
		}
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

	for (int i = 0; i < num_motors; i++) {
		bool dxl_addparam_result = false;
		uint16_t addr = ADDR_PRO_GOAL_SPEED;
		if (motors[i].model == 1040) addr = ADDR_XH_GOAL_SPEED;
		int32_t vel = rad_s2ticks_s(motors[i].command_velocity, motors[i]);
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


	return true;

}

bool bulk_read_err() {


	dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
	for (int i = 0; i<num_motors; i++) {
		bool dxl_addparam_result = false;                // addParam result
		uint16_t addr = ADDR_PRO_HARDWARE_ERROR;
		if (motors[i].model == 1040) addr = ADDR_XH_HARDWARE_ERROR;
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

	for (int i = 0; i<num_motors; i++) {
		bool dxl_getdata_result = false; // GetParam result
										 // Check if groupsyncread data of Dynamixels are available
		uint16_t addr = ADDR_PRO_HARDWARE_ERROR;
		if (motors[i].model == 1040) addr = ADDR_XH_HARDWARE_ERROR;
		dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 1);
		if (dxl_getdata_result != true)
		{
			// ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
			return false;
		}
		else {
			uint16_t addr = ADDR_PRO_HARDWARE_ERROR;
			if (motors[i].model == 1040) addr = ADDR_XH_HARDWARE_ERROR;
			motors[i].error = ticks2rads(GroupBulkRead.getData(motors[i].id, addr, 1), motors[i]);
			// ROS_INFO("[ID:%03d] Read sync pos value: %d",motors[i].id,motors[i].error);

		}
	}
	return true;
}

bool bulk_read_pos() {


	dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
	for (int i = 0; i<num_motors; i++) {
		bool dxl_addparam_result = false;                // addParam result
		uint16_t addr = ADDR_PRO_PRESENT_POSITION;
		if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_POSITION;
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
          packetHandler->printTxRxResult(dxl_comm_result);
		return false;
	}

	for (int i = 0; i<num_motors; i++) {
		bool dxl_getdata_result = false; // GetParam result
										 // Check if groupsyncread data of Dynamixels are available
		uint16_t addr = ADDR_PRO_PRESENT_POSITION;
		if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_POSITION;
		dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 4);
		if (dxl_getdata_result != true)
		{
			// ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
			return false;
		}
		else {
			uint16_t addr = ADDR_PRO_PRESENT_POSITION;
			if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_POSITION;
			motors[i].position = ticks2rads(GroupBulkRead.getData(motors[i].id, addr, 4), motors[i]);
			// ROS_INFO("[ID:%03d] Read sync pos value: %d",i,value);

		}
	}
	return true;
}


bool bulk_read_vel() {


	dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
	for (int i = 0; i<num_motors; i++) {
		bool dxl_addparam_result = false;                // addParam result
		uint16_t addr = ADDR_PRO_PRESENT_SPEED;
		if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_SPEED;
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

	for (int i = 0; i<num_motors; i++) {
		bool dxl_getdata_result = false; // GetParam result
										 // Check if groupsyncread data of Dynamixels are available
		uint16_t addr = ADDR_PRO_PRESENT_SPEED;
		if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_SPEED;
		dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 4);
		if (dxl_getdata_result != true)
		{
			// ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
			return false;
		}
		else {
			uint16_t addr = ADDR_PRO_PRESENT_SPEED;
			if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_SPEED;
			motors[i].velocity = ticks_s2rad_s(GroupBulkRead.getData(motors[i].id, addr, 4), motors[i]);
			// ROS_INFO("[ID:%03d] Read sync pos value: %d",i,value);

		}
	}
	return true;
}


bool bulk_read_load() {


	dynamixel::GroupBulkRead GroupBulkRead(portHandler, packetHandler);
	for (int i = 0; i<num_motors; i++) {
		bool dxl_addparam_result = false;                // addParam result
		uint16_t addr = ADDR_PRO_PRESENT_CURRENT;
		if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_CURRENT;
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

	for (int i = 0; i<num_motors; i++) {
		bool dxl_getdata_result = false; // GetParam result
										 // Check if groupsyncread data of Dynamixels are available
		uint16_t addr = ADDR_PRO_PRESENT_CURRENT;
		if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_CURRENT;
		dxl_getdata_result = GroupBulkRead.isAvailable(motors[i].id, addr, 2);
		if (dxl_getdata_result != true)
		{
			// ROS_ERROR("[ID:%03d] groupSyncRead isAvailable failed\n", i);
			return false;
		}
		else {
			uint16_t addr = ADDR_PRO_PRESENT_CURRENT;
			if (motors[i].model == 1040) addr = ADDR_XH_PRESENT_CURRENT;
			motors[i].current = (int16_t)GroupBulkRead.getData(motors[i].id, addr, 2) ;
			if (motors[i].model == 53768 || motors[i].model == 54024) motors[i].current *= 33000.0 / 2048.0 / 1000.0;
			else if (motors[i].model == 51200) motors[i].current *= 8250.0 / 2048.0 / 1000.0;
			else if (motors[i].model == 1040) motors[i].current *= 2.69/1000.0;
			// ROS_INFO("[ID:%03d] Read sync pos value: %d",i,value);33,000 /  2,048

		}
	}
	return true;
}


int main(int argc, char *argv[])
{

    ros::init(argc, argv, "dxl");
    ros::NodeHandle nh;

  // Initialize Packethandler2 instance
  packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION2);

  fprintf(stderr, "\n***********************************************************************\n");
  fprintf(stderr,   "*                            DXL Test                              *\n");
  fprintf(stderr,   "***********************************************************************\n\n");

  char *dev_name = (char*)DEVICENAME;

  // Initialize PortHandler instance
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  portHandler = dynamixel::PortHandler::getPortHandler(dev_name);

  // Open port
  if (portHandler->openPort())
  {
      portHandler->setBaudRate(BAUDRATE);
    printf("Succeeded to open the port!\n\n");
    printf(" - Device Name : %s\n", dev_name);
    printf(" - Baudrate    : %d\n\n", portHandler->getBaudRate());
  }
  else
  {
    printf("Failed to open the port! [%s]\n", dev_name);
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }



  uint8_t ids[] = { 1,2,3,4,5,6,7,8};

  num_motors = sizeof(ids) / sizeof(uint8_t);
  motors = (struct motor *)malloc(sizeof(struct motor)*num_motors);
  for (int i = 0; i<num_motors; i++) {
	  motors[i].id = ids[i];
	  motors[i].model = 0;
	  motors[i].protocol_ver = 2.0;
	  motors[i].command_position = 0.0;
      motors[i].command_velocity = 0.05;//
  }

//bulk_read_err();

  puts("Pinging motors...");
  for (int i = 0; i<num_motors; i++) {
      while ((!ping(motors[i]) || motors[i].model == 0)&&ros::ok()) {
		  printf("[ID:%03d] ping error\n",motors[i].id);
        ros::Rate(5).sleep();
	  }
	  printf("[ID:%03d] Ping OK, model: %d\n", motors[i].id, motors[i].model);
	  switch (motors[i].model) {
	  case 54024:
		  motors[i].cpr = 501900;
		  motors[i].gear_reduction = 502.0;
			  break;
	  case 53768:
		  motors[i].cpr = 502000;
		  motors[i].gear_reduction = 502.0;
		  break;
	  case 51200:
		  motors[i].cpr = 303800;
		  motors[i].gear_reduction = 304.0;
		  break;
	  case 29:
		  motors[i].cpr = 4096;
		  motors[i].gear_reduction = 1.0;
		  break;
	  case 1040:
		  motors[i].cpr = 4096;
		  motors[i].gear_reduction = 353.5;
		  break;
	
	 }
  }
if (!ros::ok()) exit(1);

  puts("Enabling motors torque...");
  for (int i = 0; i<num_motors; i++) {
      while (!setTorque(motors[i], true) && ros::ok()) {

		  // ROS_WARN("[ID:%03d] setTorque error",i);
         ros::Rate(5).sleep();
	  }
	  printf("[ID:%03d] Torque ON\n", i);
	  motors[i].torque = true;
  }

  if (!ros::ok()) exit(1);

  puts("Read Write loop started");

  int i = 0;
  bool first = true; 
  ros::Time pre_time;
  double rate = 0;

  while(ros::ok())
  {

pre_time=ros::Time::now();
      bool read_ok = bulk_read_pos() && bulk_read_vel() && bulk_read_load() && bulk_read_err();
	  if (read_ok) {
		  //ROS_INFO("Read OK");
		  printf("POS: ");
		  for (int i = 0; i<num_motors; i++) {
			  printf("%.3f, ", motors[i].position);
		  }
		  puts("");
		  printf("VEL: ");
		  for (int i = 0; i<num_motors; i++) {
			  printf("%.3f, ", motors[i].velocity);
		  }
		  puts("");
		  printf("CUR: ");
		  for (int i = 0; i<num_motors; i++) {
			  printf("%.3f, ", motors[i].current);
		  }
		  puts("");
		  printf("ERR: ");
		  for (int i = 0; i<num_motors; i++) {
              printf("%d, ", motors[i].error);
		  }
		  puts("");
	  }
	  else {
		    puts("Read error");
			if (first) continue;
	  }

	  if (first) {
		  for (int i = 0; i < num_motors; i++) {
              motors[i].command_position =  0;//motors[i].position;
		  }
		  first = false;
	  }

	 
	  static float s = 1;
	  for (int i = 0; i < num_motors; i++) {
          motors[i].command_position+= 0.0005*s;
		  if (motors[i].command_position > 0.1) s = -1;
		  else if (motors[i].command_position < -0.1) s = 1;
	  }
	//  printf("command= %.3f\n", motors[0].command_position);
      printf("DIF: ");
	  for (int i = 0; i<num_motors; i++) {
          printf("%.3f, ", motors[i].command_position-motors[i].position);
	  }
      puts("");


      bool write_ok = bulk_write();
	  if (write_ok) {
		  //ROS_INFO("Write OK");
	  }
	  else puts("Write error");

   ros::Rate(100).sleep();

      double dt = (ros::Time::now() - pre_time).toSec(); //the time, in ms, that took to render the scene
      if (dt > 0)
         rate=1.0/dt;
     std::cout << "rate: "<<rate<<std::endl;
	  puts("");
     ros::spinOnce();
  }
  portHandler->closePort();
}
