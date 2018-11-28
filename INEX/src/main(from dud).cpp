#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/TeleportAbsolute.h"
#include "nav_msgs/OccupancyGrid.h"
#include <iostream>
#include <fstream>
#include <std_msgs/Header.h>


void test_save_my_map() {
  // create some fake map data
    std::ifstream myfile ("/home/dud/Desktop/myMap.pmg");
    int8_t image[] = ;
    std::cout << image;

	//nav_msgs::OccupancyGrid* newGrid = map.Grid();
	//newGrid->header = header;
	
    myfile << image;
    myfile.close();
    }
  fakeData = { header = myFakeHeader, data = myFakeData }
  // call savemymap with fake data
  save_my_map(fakeData);
  // see if data is being saved
  if(fileContains(myFakeData) {
	// we did well
  } else {
   // we failed..
  }
}

void save_my_map(const nav_msgs::OccupancyGrid::ConstPtr& mMap){
   std::ofstream myfile ("/home/dud/ros_ws/src/INEX/map/mars_map.pgm");
    if (myfile.is_open())
    {
	int8_t image[] = mMap->data;
	std::cout << image 

	//nav_msgs::OccupancyGrid* newGrid = map.Grid();
	//newGrid->header = header;
	
	myfile << image;
        myfile.close();
    }
    else std::cout << "Unable to open file";
}
int main(int argc, char *argv[]) {
  ros::init(argc, argv, "inex");
  ros::NodeHandle nh;
  //get the map and into raw_map
  nav_msgs::OccupancyGrid raw_map;
  ros::Subscriber map = nh.subscribe("map", 200, save_my_map); //"nav_msgs/OccupancyGrid", 2, save_my_map);

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

