#include <ros/ros.h>
#include <iostream>
#include "std_msgs/Char.h"

//this node sends the messages to the boost
int main(int argc, char *argv[]) {
    ros::init(argc, argv, "remote");
    ros::NodeHandle nh;
    ros::Publisher cmd_pub = nh.advertise<std_msgs::Char>("command_topic", 10);
    std_msgs::Char command;
    while (ros::ok) {
	    ros::spinOnce();
            ROS_INFO("commands > a = auto,\tm = manual,\tt = transmit,\tq = quit\n");
            std::cin >> command.data;
            if (command.data == 'a' || command.data == 'A'){
                ROS_INFO("requesting autonumous navigation");
                cmd_pub.publish(command);
	    }
            else if (command.data == 'm' || command.data == 'M'){
                ROS_INFO("requesting manual navigation");
                cmd_pub.publish(command);
	    }
            else if (command.data == 't' || command.data == 'T'){
                ROS_INFO("requesting data transmission");
                cmd_pub.publish(command);
		ros::Duration(10.0).sleep();
                std::system("/home/wizard/Documents/Livros/Escola/AAU/P1/Development/Ros/src/remote/src/get.sh");
	    }
            else if (command.data == 'q' || command.data == 'Q'){
                ROS_INFO("Now I'm going to catch some Z's ... ZZZ ZZZ ZZZ");
                cmd_pub.publish(command);
                break;
            }
        }
    }
