#include "ros/ros.h"
#include <iostream>
#include <math.h>
#include <vector>
#include "nav_msgs/OccupancyGrid.h"
#include <tf/transform_listener.h>
#include <geometry_msgs/PointStamped.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

//This render a map and decides the exploration path
int distance = 10;
int complexity = 4;
nav_msgs::OccupancyGrid::ConstPtr mMap;
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
int mainX = 0;
int mainY = 0;
bool debug = true;

void set_map(const nav_msgs::OccupancyGrid::ConstPtr& m){
    if (debug) ROS_INFO("\tbreakpoint - set_map 1");
    //This recieves an OccupancyGrid pointer and makes it global to be used in
    //"goodPoint", besides that, it renders an updated 2D image of the map.
    mMap = m;
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

bool goodPoint(int x, int y){
    if (debug) ROS_INFO("\tbreakpoint - goodPoint 1");
    unsigned int coast = x + (mMap->info.height - y - 1) * mMap->info.width;
    if (debug) ROS_INFO("\tbreakpoint - goodPoint 2");
    //first check if it is out of the map borders or if inside is not maped
    if (mMap->info.height < y && mMap->info.width < x || mMap->data[coast] != 0 && mMap->data[coast] <= 100) return true; else return false;
}

std::vector<int> getAdjacent(int x, int y){ //, std::vector<int> missedPoints){
    //Finds the points arround the target, and sort them
    // clockwise, lower, current level, upper
    if (debug) ROS_INFO("\tbreakpoint - getAdjacent 1");
    int n;
    std::vector<int> adjacent, holder;
    double angle;
    //Characterize the current position
    int L = sqrt(pow(x, 2) + pow(y, 2)) / distance;
    int N = L * complexity;
    double teta;
    if (debug) ROS_INFO("\tbreakpoint - getAdjacent 2");
    if (x == 0){
	if (y == 0) teta = 0;
	else if (y > 0) teta = M_PI / 2;
	else if (y < 0) teta = M_PI * 3 / 2;
    }
    else teta = atan(y / x);
    double delta = 2 * M_PI / N;
    //Get the points from the lower level inside the angle
    if (L > 0){
    n = (L - 1) * complexity;
    if (debug) ROS_INFO("\tbreakpoint - getAdjacent 3");
    for (int i = 0; i < n; i++){
        angle = i * 2 * M_PI / n;
        if (angle <= teta + delta && angle >= teta - delta){
            holder.push_back(int((L - 1) * distance * cos(angle)));
            holder.push_back(int((L - 1) * distance * sin(angle)));
        }
    }
    if (debug) ROS_INFO("\tbreakpoint - getAdjacent 4");
    //get the 2 adjacent points in the same level
    holder.push_back(int(L * distance * cos(teta + delta)));
    holder.push_back(int(L * distance * sin(teta + delta)));
    holder.push_back(int(L * distance * cos(teta - delta)));
    holder.push_back(int(L * distance * sin(teta - delta)));
    };
    //get the upper level points in angle
    n = (L + 1) * complexity;
    if (debug) ROS_INFO("\tbreakpoint - getAdjacent 5");
    for (int i = 0; i < n; i++){
        angle = i * 2 * M_PI / n;
        if (angle <= teta + delta && angle >= teta - delta){
            holder.push_back(int((L + 1) * distance * cos(angle)));
            holder.push_back(int((L + 1) * distance * sin(angle)));
        }
    }
    if (debug) ROS_INFO("\tbreakpoint - getAdjacent 6");
    //erase repeated points
    adjacent.push_back(holder[0]);
    adjacent.push_back(holder[1]);
    for (int i = 2; i < holder.size(); i += 2)
        if (holder[i] != holder[i - 2] && holder[i + 1] != holder[i - 1]){
            adjacent.push_back(holder[i]);
            adjacent.push_back(holder[i - 1]);
        };// else ROS_INFO("not eficient yet, getAdjacent repeats a point");
    if (debug) ROS_INFO("\tbreakpoint - getAdjacent 7");
    //for (int i= 0; i < holder.size() - 1; i += 2) std::cout << "\n\t" << holder[i] << ", " << holder[i + 1] << std::endl;
    return holder;
}

std::vector<int> think(float x, float y){
    if (debug) ROS_INFO("\tbreakpoint - think 1");
    //recieves the last point and seach for the next
    std::vector<int> next(2);
    std::vector<int> adjacent = getAdjacent(x, y);
    if (debug) ROS_INFO("\tbreakpoint - think 2");
    //if the point is not mapped, is an objective
    for (int point = 0; point <= adjacent.size(); point += 2) {
        std::cout << "\n............checking if point is good: " << adjacent[point] << ", " << adjacent[point + 1] << std::endl;
        if (goodPoint(adjacent[point], adjacent[point + 1])) {
            next[0] = adjacent[point];
            next[1] = adjacent[point + 1];
        std::cout << "\n............in" << std::endl; 
            return next;
        };
    };
    if (debug) ROS_INFO("\tbreakpoint - think 3");
    //expand search in the radios of the current point
    int counter = 0;
    while (next.empty() && counter < 100) {
        if (goodPoint(x + counter * distance, y + counter * distance)){
            next[0] = x + counter * distance;
            next[1] = y + counter * distance;
        } else if (goodPoint(x - counter * distance, y - counter * distance)) {
            next[0] = x - counter * distance;
            next[1] = y - counter * distance;
        } else if (goodPoint(x - counter * distance, y + counter * distance)) {
            next[0] = x - counter * distance;
            next[1] = y + counter * distance;
        } else if (goodPoint(x + counter * distance, y - counter * distance)) {
            next[0] = x + counter * distance;
            next[1] = y - counter * distance;
        }
        counter ++;
    }
    if (debug) ROS_INFO("\nbreakpoint - think 4");
    return next;
}

bool set_goal(int x, int y){
    MoveBaseClient ac("move_base", true);
    while(!ac.waitForServer(ros::Duration(5.0))){
        ROS_INFO("Waiting for the move_base action server to come up");
    }
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.frame_id = "base_link";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = x;
    goal.target_pose.pose.position.y = y;
    std::cout << "my goal is " << x << ", " << y << "\n"; 
    ROS_INFO("Let me plan my path");
    ac.sendGoal(goal);
    ac.waitForResult();
    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
        ROS_INFO("Success, moving to the next point");
        return true;
    }
    else{
        ROS_INFO("Something on my way, i'll go arround...");
        return false;
    };
return true;
}

int main(int argc, char *argv[]) {
    ros::init(argc, argv, "navigator");
    ROS_INFO("\n\n\tHello Mars! Let's find the E.T.\n\n");
    ros::NodeHandle nh;
    ros::Subscriber map = nh.subscribe("map", 1, set_map);
    ros::spinOnce();
    //tf::TransformListener listener;
    //tf::StampedTransform tran;
    //listener.lookupTransform("/move_base/local_costmap/global_frame",
    //                         "/move_base/local_costmap/robot_base_frame", ros::Time::now(), tran);
    if (debug) ROS_INFO("breakpoint - Main 1");

    //while(ros::ok){
        std::vector<int> next = think(mainX, mainY); //tran.getOrigin().x(), tran.getOrigin().y());
        ROS_INFO("breakpoint - Main 2");
	std::cout << "..............." << next[0] << ", " << next[1] << "\n";
        set_goal(next[0], next[1]);
        ROS_INFO("breakpoint - Main 3");
        ros::spin();
    //}
}
