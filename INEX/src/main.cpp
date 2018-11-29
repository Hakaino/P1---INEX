#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/TeleportAbsolute.h"
#include "nav_msgs/OccupancyGrid.h"
#include <iostream>
#include <fstream>
#include <std_msgs/Header.h>

void save_my_map(const nav_msgs::OccupancyGrid::ConstPtr& mMap){
	std::string  mapName = "/home/dud/ros_ws/src/INEX/map/mars_map.pgm";
	ROS_INFO("\nNow I will draw the map");
	FILE* mapFile = fopen(mapName.c_str(), "w");
	if (!mapFile){
		ROS_ERROR("\nSorry!!!....I could not save the map :(");
		return;
	}
	fprintf(mapFile, "\P5\n# Mars map, Space program: C2-20, Mission: RobP1-18 %.3f m/pix\n%d %d\n255\n",
		mMap->info.resolution, mMap->info.width, mMap->info.height);
	for(unsigned int y = 0; y < mMap->info.height; y++){
		for(unsigned int x = 0; x < mMap->info.width; x++){
			unsigned int i = x + (mMap->info.height - y - 1) * mMap->info.width;
			if (mMap->data[i] == 0){
				fputc(254, mapFile);
			} else if (mMap->data[i] == +100){
				fputc(000, mapFile);
			} else {
				fputc(205, mapFile);
			}
		}
	}
	fclose(mapFile);
	ROS_INFO("\nI saved the map ^_^");
}

int main(int argc, char *argv[]){
	ros::init(argc, argv, "inex");
	ros::NodeHandle nh;
	//this subscribes to the topic map from map_server
	ros::Subscriber map = nh.subscribe("map", 20, save_my_map);

  /*
  ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/navi", 10);

  //get the map and into raw_map
  nav_msgs::OccupancyGrid raw_map;
  ros::Subscriber map = nh.subscribe("nav_msgs/OccupancyGrid", 2, save_my_map);

  ros::Duration(1.0).sleep();

  /*geometry_msgs::Twist msg;
  msg.linear.x = 1.0;
  cmd_vel_pub.publish(msg);

  ros::Duration(1.0).sleep();

  msg.linear.x = 0.0;
  msg.angular.z = 1.0;
  cmd_vel_pub.publish(msg);

  ros::Duration(1.0).sleep();*/
  ros::spin();

  return 0;
}
