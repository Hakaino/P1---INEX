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

void set_map(const nav_msgs::OccupancyGrid::ConstPtr& m){
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

bool goodPoint(int x, int y){
    unsigned int coast = x + (mMap->info.height - y - 1) * mMap->info.width;
    if (mMap->data[coast] != 0 && mMap->data[coast] <= 100){
        //std::string text = "\nThe point (%n, %n) is my next goal!", x, y;
        //ROS_INFO(text);
        return true;
    };
    return false;
}

std::vector<int> getAdjacent(int x, int y){ //, std::vector<int> missedPoints){
    //Finds the points arround the target, and sort them
    // clockwise, lower, current level, upper
    int n;
    std::vector<int> adjacent, holder;
    double angle;
    //Characterize the current position
    int L = sqrt(pow(x, 2) + pow(y, 2)) / distance;
    int N = L * complexity;
    double teta;
    if (x == 0 && y > 0) teta = M_PI; else if (x == 0 && y > 0) teta = M_PI * 3 / 2; else teta = atan(y / x);
    double delta = 2 * M_PI / N;
    //Get the points from the lower level inside the angle
    n = (L - 1) * complexity;
    for (int i = 0; i < n; i++){
        angle = i * 2 * M_PI / n;
        if (angle <= teta + delta && angle >= teta - delta){
            holder.push_back(int((L - 1) * distance * cos(angle)));
            holder.push_back(int((L - 1) * distance * sin(angle)));
        }
    }
    //get the 2 adjacent points in the same level
    holder.push_back(int(L * distance * cos(teta + delta)));
    holder.push_back(int(L * distance * sin(teta + delta)));
    holder.push_back(int(L * distance * cos(teta - delta)));
    holder.push_back(int(L * distance * sin(teta - delta)));
    //get the upper level points in angle
    n = (L + 1) * complexity;
    for (int i = 0; i < n; i++){
        angle = i * 2 * M_PI / n;
        if (angle <= teta + delta && angle >= teta - delta){
            holder.push_back(int((L + 1) * distance * cos(angle)));
            holder.push_back(int((L + 1) * distance * sin(angle)));
        }
    }
    //erase repeated points
    adjacent.push_back(holder[0]);
    adjacent.push_back(holder[1]);
    for (int i = 2; i < holder.size(); i += 2)
        if (holder[i] != holder[i - 2] && holder[i + 1] != holder[i - 1]){
            adjacent.push_back(holder[i]);
            adjacent.push_back(holder[i - 1]);
        } else std::cout << "not eficient yet, it repeats this point: " << holder[i] << ", " << holder[i + 1] << "\n";
    return holder;
}

std::vector<int> think(int x, int y){
    //recieves the last point and seach for the next
    bool clockwise;
    std::vector<int> next(2);
    std::vector<int> adjacent = getAdjacent(x, y);
    //if the point is not mapped, is an objective
    for (int point = 0; point <= adjacent.size(); point += 2) {
        if (goodPoint(adjacent[point], adjacent[point + 1])) {
            next[0] = adjacent[point];
            next[1] = adjacent[point + 1];
            return next;
        };
    };
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
    return next;
}

bool set_goal(std::vector<int> point){
    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);

    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))){
        ROS_INFO("Waiting for the move_base action server to come up");
    }

    move_base_msgs::MoveBaseGoal goal;

    //we'll send a goal to the robot to move 1 meter forward
    goal.target_pose.header.frame_id = "base_link";
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.pose.position.x = point[0];
    goal.target_pose.pose.position.y = point[1];
    //goal.target_pose.pose.orientation.w = 1.0;
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
}

int main(int argc, char *argv[]) {
    ros::init(argc, argv, "navigator");
    ros::NodeHandle nh;
    ros::Subscriber map = nh.subscribe("map", 5, set_map);
    //ros::Publisher goal = nh.advertise();
    //geometry_msgs::PointStamped
    tf::TransformListener listener;
    tf::StampedTransform tran;
    listener.lookupTransform("/move_base/local_costmap/global_frame",
                             "/move_base/local_costmap/robot_base_frame", ros::Time::now(), tran);
    while(ros::ok){
        std::vector<int> next = think(tran.getOrigin().x(), tran.getOrigin().y());
        set_goal(next);
        ros::spin();
    }
}
