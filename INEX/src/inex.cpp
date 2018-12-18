#include "ros/ros.h"
#include "nav_msgs/OccupancyGrid.h"
#include <iostream>
#include "std_msgs/Char.h"
#include "sensor_msgs/BatteryState.h"


void executer(const std_msgs::Char::ConstPtr& key){
  //changes betwee manual and autonumous navigation
  if (key->data == 'a' || key->data == 'A') {
	std::system("/home/dud/ros_ws/src/INEX/src/auto.sh");
	ROS_INFO("[Autonumous mode activate]");
  }
  else if (key->data == 'm' || key->data == 'M') {
	std::system("/home/dud/ros_ws/src/INEX/src/manual.sh");
	ROS_INFO("[Manual mode activate]");
  }
  else if (key->data == 't' || key->data == 'T') {
	std::system("/home/dud/ros_ws/src/INEX/src/compress.sh");
	ROS_INFO("[Compressing data for transmission]");
  };
}

void low_power(const sensor_msgs::BatteryState::ConstPtr& power){
  //if the power is too low, kill all movement
  if (power->percentage < 0.1) std::system("/home/dud/src/INEX/src/power.sh");
}

void set_map(const nav_msgs::OccupancyGrid::ConstPtr& mMap){
    //This renders an updated 2D image of the map.
    std::string  mapName = "/home/dud/ros_ws/src/INEX/map/mars_map.pgm";
    ROS_INFO("\nNow I will draw the map");
    FILE* mapFile = fopen(mapName.c_str(), "w");
    if (!mapFile){
        ROS_ERROR("\nSorry!!!....I could not save the map :(");
        return;
    }
    fprintf(mapFile, "P5\n# Mars map, Space program: C2-20, Mission: RobP1-18 %.3f m/pix\n%d %d\n255\n",
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

int main(int argc, char *argv[]) {
    ros::init(argc, argv, "inex");
    ros::NodeHandle nh;
    ros::Subscriber keys = nh.subscribe("command_topic", 5, executor);
    ros::Subscriber map = nh.subscribe("map", 5, set_map);
    ros::Subscriber power = nh.subscribe("sensor_msgs/BatteryState", 5, low_power);
    ros::spin();
}
