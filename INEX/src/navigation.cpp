#include "ros/ros.h"
#include <iostream>
#include <math.h>
#include <vector>
#include "nav_msgs/OccupancyGrid.h"

int distance = 10;
int complexity = 4;
nav_msgs::OccupancyGrid::ConstPtr mMap; //map pointer

void set_map(const nav_msgs::OccupancyGrid::ConstPtr& m){
    mMap = m;
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

int* think(int x, int y){
    //recieves the last point and seach for the next
    bool clockwise;
    int next[2];
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
    while (counter < 100) {
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
        counter += distance;
    }
    return next;
    //TODO: translate central point to current point
    //TODO: find the next point with the previous algorithm
    //TODO:translate the center to the initial one and continue the search
}

int main(int argc, char *argv[]) {
    ros::init(argc, argv, "explorer");
    ros::NodeHandle nh;
    ros::Subscriber map = nh.subscribe("map", 5, set_map);
    ros::spin();
}
