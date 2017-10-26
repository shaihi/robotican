
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>   // std::cout
#include <string>     // std::string, std::to_string
#include <vector>
#include <time.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/joint_state_interface.h>

#include <hardware_interface/robot_hw.h>
#include <controller_manager/controller_manager.h>
#include <hardware_interface/posvel_command_interface.h>
#include "dynamixel_sdk/dynamixel_sdk.h"                                  // Uses Dynamixel SDK library
#include <ros/package.h>
#include "yaml-cpp/yaml.h"

#include <ros/ros.h>
#include <robotican_hardware_interface/Device.h>
#include <robotican_hardware_interface/RiCBoardManager.h>

#ifdef HAVE_NEW_YAMLCPP
// The >> operator disappeared in yaml-cpp 0.5, so this function is
// added to provide support for code written under the yaml-cpp 0.3 API.
template<typename T>
void operator >> (const YAML::Node& node, T& i)
{
    i = node.as<T>();
}

#endif

struct dynamixel_spec
{
    std::string name;
    uint16_t model_number;
    int cpr;
    double rpm_scale_factor;
    float torque_constant_a;
    float torque_constant_b;
};


// Control table address FOR MX-28
#define ADDR_MX_MODEL_NUM               30
#define ADDR_MX_TORQUE_ENABLE           64                  // Control table address is different in Dynamixel model
#define ADDR_MX_GOAL_POSITION           116
#define ADDR_MX_PROFILE_VELOCITY        112
#define ADDR_MX_PRESENT_POSITION        132
#define ADDR_MX_PRESENT_SPEED           128
#define ADDR_MX_PRESENT_LOAD         126
#define ADDR_MX_PRESENT_TEMPERATURE     146
#define ADDR_MX_MOVING                  122
#define ADDR_MX_HARDWARE_ERROR          70

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
#define ADDR_XH_MODEL_NUM               1040
#define ADDR_XH_TORQUE_ENABLE           64                  // Control table address is different in Dynamixel model
#define ADDR_XH_GOAL_POSITION           116
#define ADDR_XH_GOAL_SPEED              104
#define ADDR_XH_VELOCITY_LIMIT          44
#define ADDR_XH_PROFILE_VELOCITY       112
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


struct motor
{
    uint8_t id;
    uint16_t model;
    bool torque;
    double position;
    double velocity;
    double current;
    double effort;
    double command_position;
    double command_velocity;
    double pre_command_velocity;
    uint8_t error;
    int cpr;
    double rpm_scale_factor;
    float protocol_ver;
    std::string joint_name;
    std::string interface;
    float torque_constant_a;
    float torque_constant_b;
bool first_read;

};
