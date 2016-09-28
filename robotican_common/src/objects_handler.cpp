

#include <ros/ros.h>
#include <std_srvs/SetBool.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>
#include <ar_track_alvar_msgs/AlvarMarkers.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/CollisionObject.h>
#include <iostream>
#include <sstream>
#include "tf/tf.h"


bool update_cb(std_srvs::SetBool::Request  &req,std_srvs::SetBool::Response &res);
void handle_objects(const ar_track_alvar_msgs::AlvarMarkers::ConstPtr& markers);
std::string get_obj_id(int id);
shape_msgs::SolidPrimitive get_obj_shape(int id);
geometry_msgs::Pose get_obj_position(geometry_msgs::Pose  pose);

bool update=false;



moveit::planning_interface::PlanningSceneInterface *planning_scene_interface_ptr;




int main(int argc, char **argv) {
    ros::init(argc, argv, "objects_handler");
    ros::NodeHandle n;
   // ros::NodeHandle pn("~");

    //TODO: READ OBJECTS DB
    //std::string object_id;
    //pn.param<std::string>("object_id", obcol_objectsject_id, "can");

    ros::Subscriber ar_sub = n.subscribe("ar_pose_marker", 10, handle_objects);
    ros::Subscriber color_sub = n.subscribe("detected_objects", 10, handle_objects);

   //
    ros::ServiceServer service = n.advertiseService("update_collision_objects", update_cb);


    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;
    planning_scene_interface_ptr=&planning_scene_interface;

while (ros::ok()){
    //TODO: RATE+TIMEOUT
    //if (update) planning_scene_interface.removeCollisionObjects();

    ros::spinOnce();
}
    return 0;
}

std::string get_obj_id(int id) { //TODO: FROM DB

    std::ostringstream os;
    os<<"object_";
      os << id;
    return os.str();
}

shape_msgs::SolidPrimitive get_obj_shape(int id) { //TODO: FROM DB

shape_msgs::SolidPrimitive object_primitive;
object_primitive.type = object_primitive.CYLINDER;
object_primitive.dimensions.resize(2);
object_primitive.dimensions[0] = 0.17;
object_primitive.dimensions[1] = 0.03;

    return object_primitive;
}

geometry_msgs::Pose get_obj_position(geometry_msgs::Pose  pose){ //TODO: FROM DB

    pose.position.x+=0.015;
    return pose;
 }

void handle_objects(const ar_track_alvar_msgs::AlvarMarkers::ConstPtr& markers)
{
    std::vector<moveit_msgs::CollisionObject> col_objects;

    for (int i=0;i<markers->markers.size();i++) {

        ar_track_alvar_msgs::AlvarMarker m=markers->markers[i];

        moveit_msgs::CollisionObject obj;
        obj.header.frame_id = m.header.frame_id;
        obj.id = get_obj_id(m.id);
        obj.primitives.push_back(get_obj_shape(m.id));

        obj.primitive_poses.push_back(get_obj_position(m.pose.pose));
        obj.operation=obj.ADD;

        col_objects.push_back(obj);

        if (update) planning_scene_interface_ptr->addCollisionObjects(col_objects);

    }
}


bool update_cb(std_srvs::SetBool::Request  &req,
               std_srvs::SetBool::Response &res) {

    update=req.data;
    res.success=true;
    if (update)  res.message="update collision is ON";
    else res.message="update collision is OFF";
    return true;
}
