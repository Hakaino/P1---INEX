#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/TeleportAbsolute.h"
#include "nav_msgs/OccupancyGrid.h"
#include <iostream>
#include <fstream>
#include <std_msgs/Header.h>

void save_my_map(const nav_msgs::OccupancyGrid::ConstPtr& mMap){
	/*This recieves an OccupancyGrid object and generate a map,
	it updates everytime that there are changes.
	it draws the file pixel by pixel.*/
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
	ros::Subscriber map = nh.subscribe("map", 20, save_my_map);
  ros::spin();

  return 0;
}
