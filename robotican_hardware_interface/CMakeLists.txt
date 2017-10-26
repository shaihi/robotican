cmake_minimum_required(VERSION 2.8.3)
project(robotican_hardware_interface)

add_definitions(-DPC_SIDE)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED dynamic_reconfigure COMPONENTS
        roscpp
        dynamic_reconfigure
        tf
        dynamixel_sdk
        controller_manager
gazebo_ros_control
control_toolbox
ric_board
message_generation
serial

        )

find_package(gazebo REQUIRED)

## System dependencies are found with CMake's conventions
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

add_message_files(
        FILES
        JointEnable.msg
        ChainEnable.msg
        JointLimits.msg
        ChainLimits.msg
        imuClib.msg
)

## Generate services in the 'srv' folder
add_service_files(
        FILES
        setImuClib.srv
       # TorsoHomming.srv
        #   Service2.srv
)
generate_messages(
        DEPENDENCIES
        sensor_msgs
        std_msgs
        geometry_msgs
)
generate_dynamic_reconfigure_options(
        cfg/RiCBoard.cfg #MOTOR
        cfg/RiCBoardServo.cfg
        cfg/RiCBoardPotentiometer.cfg
)



pkg_check_modules(yaml-cpp REQUIRED yaml-cpp)
add_definitions(-DPC_SIDE)
if(NOT ${yaml-cpp_VERSION} VERSION_LESS "0.5")
 add_definitions(-DHAVE_NEW_YAMLCPP)
endif(NOT ${yaml-cpp_VERSION} VERSION_LESS "0.5")


catkin_package(
        INCLUDE_DIRS include
LIBRARIES ${PROJECT_NAME} armadillo2_hardware_interface_sim 

)


add_library(robotican_hardware_interface
        include/${PROJECT_NAME}/Protocol.h
               include/${PROJECT_NAME}/TransportLayer.h
               src/${PROJECT_NAME}/TransportLayer.cpp
               include/${PROJECT_NAME}/RiCBoardManager.h
               src/${PROJECT_NAME}/RiCBoardManager.cpp
               src/${PROJECT_NAME}/ros_utils.cpp
               include/${PROJECT_NAME}/Device.h
               src/${PROJECT_NAME}/Device.cpp
               include/${PROJECT_NAME}/Battery.h
               src/${PROJECT_NAME}/Battery.cpp
               include/${PROJECT_NAME}/Servo.h
               src/${PROJECT_NAME}/Servo.cpp
               include/${PROJECT_NAME}/Ultrasonic.h
               src/${PROJECT_NAME}/Ultrasonic.cpp
               include/${PROJECT_NAME}/Gps.h
               src/${PROJECT_NAME}/Gps.cpp
               include/${PROJECT_NAME}/Imu.h
               src/${PROJECT_NAME}/Imu.cpp
               include/${PROJECT_NAME}/RiCMotor.h
               src/${PROJECT_NAME}/RiCMotor.cpp
               include/${PROJECT_NAME}/Switch.h
               src/${PROJECT_NAME}/Switch.cpp
               include/${PROJECT_NAME}/Relay.h
               src/${PROJECT_NAME}/Relay.cpp
)

add_library(
    armadillo2_hardware_interface_sim
        include/${PROJECT_NAME}/armadillo2_sim_interface.h
        src/armadillo2_sim_interface.cpp
        )
include_directories(include
        ${catkin_INCLUDE_DIRS}
        ${yaml-cpp_INCLUDE_DIRS}
${GAZEBO_INCLUDE_DIRS}
${Boost_INCLUDE_DIR}
        )



add_executable(armadillo2_hardware_interface_node src/armadillo2_hardware_interface.cpp)
add_dependencies(armadillo2_hardware_interface_node ${PROJECT_NAME}_gencfg)

add_dependencies(robotican_hardware_interface ${PROJECT_NAME} ${${PROJECT_NAME}_gencpp} ${${PROJECT_NAME}_EXPORTED_TARGETS})

target_link_libraries(robotican_hardware_interface ${catkin_LIBRARIES} ${yaml-cpp_LIBRARIES})

target_link_libraries(armadillo2_hardware_interface_node ${catkin_LIBRARIES} robotican_hardware_interface)

install(TARGETS armadillo2_hardware_interface_sim
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        )

install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
        )


install(FILES armadillo2_hw_sim_plugins.xml
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
