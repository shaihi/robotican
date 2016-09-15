#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_srvs/Trigger.h>
#include <geometry_msgs/PointStamped.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <std_msgs/Empty.h>
#include <tf/transform_listener.h>
#include "tf/message_filter.h"
#include "message_filters/subscriber.h"
#include <actionlib/client/simple_action_client.h>
#include <control_msgs/GripperCommandAction.h>
#include <moveit/move_group_interface/move_group.h>
#include <moveit/kinematics_metrics/kinematics_metrics.h>
#include <moveit_msgs/WorkspaceParameters.h>
#include <moveit/trajectory_processing/iterative_time_parameterization.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <std_srvs/SetBool.h>
#include <moveit_msgs/PickupAction.h>

typedef actionlib::SimpleActionClient<moveit_msgs::PickupAction> PickClient;

void look_down();
bool set_collision_update(bool state);

moveit_msgs::PickupGoal BuildPickGoal(const std::string &objectName);

ros::ServiceClient *uc_client_ptr;
ros::Publisher pub_controller_command;

void look_down() {

    trajectory_msgs::JointTrajectory traj;
    traj.header.stamp = ros::Time::now();
    traj.joint_names.push_back("head_pan_joint");
    traj.joint_names.push_back("head_tilt_joint");
    traj.points.resize(1);
    traj.points[0].time_from_start = ros::Duration(1.0);
    std::vector<double> q_goal(2);
    q_goal[0]=0.0;
    q_goal[1]=0.6;
    traj.points[0].positions=q_goal;
    traj.points[0].velocities.push_back(0);
    traj.points[0].velocities.push_back(0);
    pub_controller_command.publish(traj);
}

int main(int argc, char **argv) {

    ros::init(argc, argv, "pick_and_plce_node");
    ros::AsyncSpinner spinner(2);
    spinner.start();

    ros::NodeHandle n;
    ros::NodeHandle pn("~");
    std::string startPositionName;
    pn.param<std::string>("start_position_name", startPositionName, "pre_grasp2");

    ROS_INFO("Hello");
    moveit::planning_interface::MoveGroup group("arm");
    //Config move group
    group.setMaxVelocityScalingFactor(0.1);
    group.setMaxAccelerationScalingFactor(0.5);
    group.setPlanningTime(5.0);
    group.setNumPlanningAttempts(10);
    group.setPlannerId("RRTConnectkConfigDefault");
    group.setPoseReferenceFrame("base_footprint");

    group.setStartStateToCurrentState();
    group.setNamedTarget(startPositionName);
    moveit::planning_interface::MoveGroup::Plan startPosPlan;
    if(group.plan(startPosPlan)) { //Check if plan is valid
        group.execute(startPosPlan);
        pub_controller_command = n.advertise<trajectory_msgs::JointTrajectory>("/pan_tilt_trajectory_controller/command", 2);
        ROS_INFO("Waiting for the moveit action server to come up");

        std::string uc="/update_collision/object";
        ros::ServiceClient uc_client = n.serviceClient<std_srvs::SetBool>(uc);
        ROS_INFO("Waiting for update_collision service...");
        uc_client.waitForExistence();
        uc_client_ptr = &uc_client;
        set_collision_update(true);
        ros::Duration(5.0).sleep();
        look_down();
        ros::Duration(5.0).sleep();
        set_collision_update(false);
        ROS_INFO("Looking down...");
        ROS_INFO("Ready!");

        PickClient pickClient("pickup", true);
        pickClient.waitForServer();
        std::string objectName = "can";
        moveit_msgs::PickupGoal goal = BuildPickGoal(objectName);
        pickClient.sendGoalAndWait(goal);
    }
    else {
        ROS_ERROR("Error");
    }
    ros::waitForShutdown();
    return 0;
}

moveit_msgs::PickupGoal BuildPickGoal(const std::string &objectName) {
    moveit_msgs::PickupGoal goal;
    goal.target_name = objectName;
    goal.group_name = "arm";
    goal.end_effector = "eef";
    goal.allowed_planning_time = 5.0;
    goal.planning_options.replan_delay = 2.0;
    goal.planning_options.planning_scene_diff.is_diff = true;
    goal.planning_options.planning_scene_diff.robot_state.is_diff = true;
    goal.planning_options.replan=true;
    goal.planning_options.replan_attempts=5;
    goal.planner_id = "RRTConnectkConfigDefault";

    goal.minimize_object_distance = true;
    moveit_msgs::Grasp g;
    g.max_contact_force = 0.01;
    g.grasp_pose.header.frame_id = goal.target_name;
    g.grasp_pose.pose.position.x = -0.2;
    g.grasp_pose.pose.position.y = 0.0;
    g.grasp_pose.pose.position.z = 0.0;
    g.grasp_pose.pose.orientation.x = 0.0;
    g.grasp_pose.pose.orientation.y = 0.0;
    g.grasp_pose.pose.orientation.z = 0.0;
    g.grasp_pose.pose.orientation.w = 1.0;

    g.pre_grasp_approach.direction.header.frame_id = "/base_footprint";
    g.pre_grasp_approach.direction.vector.x = 1.0;
    g.pre_grasp_approach.min_distance = 0.1;
    g.pre_grasp_approach.desired_distance = 0.2;

    g.post_grasp_retreat.direction.header.frame_id = "/base_footprint";
    g.post_grasp_retreat.direction.vector.z = 1.0;
    g.post_grasp_retreat.min_distance = 0.1;
    g.post_grasp_retreat.desired_distance = 0.2;

    g.pre_grasp_posture.joint_names.push_back("left_finger_joint");
    g.pre_grasp_posture.joint_names.push_back("right_finger_joint");
    g.pre_grasp_posture.points.resize(1);
    g.pre_grasp_posture.points[0].positions.resize(g.pre_grasp_posture.joint_names.size());
    g.pre_grasp_posture.points[0].positions[0] = -0.56;
    g.pre_grasp_posture.points[0].positions[1] = 0.56;
    g.pre_grasp_posture.points[0].time_from_start = ros::Duration(1.0);

    g.grasp_posture.joint_names = g.pre_grasp_posture.joint_names;
    g.grasp_posture.points.resize(1);
    g.grasp_posture.points[0].positions.resize(g.grasp_posture.joint_names.size());
    g.grasp_posture.points[0].positions[0] = 0.25;
    g.grasp_posture.points[0].positions[1] = -0.25;
    g.grasp_posture.points[0].effort.resize(g.grasp_posture.joint_names.size());
    g.grasp_posture.points[0].effort[0] = 0.02;
    g.grasp_posture.points[0].effort[1] = 0.02;
    g.grasp_posture.points[0].time_from_start = ros::Duration(25.0);
    goal.possible_grasps.push_back(g);
    return goal;
}

bool set_collision_update(bool state){
    std_srvs::SetBool srv;
    srv.request.data=state;
    if (uc_client_ptr->call(srv))
    {
        ROS_INFO("update_colision response: %s", srv.response.message.c_str());
        return true;
    }
    else
    {
        ROS_ERROR("Failed to call service /find_objects_node/update_colision");
        return false;
    }

}
