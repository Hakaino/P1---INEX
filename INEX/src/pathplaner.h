#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <iostream>
#include <map>
#include <cmath>
#include <math.h>
#include <queue>
#include <vector>

int distance = 10;
int complexity = 4; //the bigger the number, the roundest will be the path

std::vector<int[2]> getAdjacent(int lastPoint[2]){ //, std::vector<int[2]> missedPoints){
    std::vector<int[2]> adjacent;
    //Characterize the current position
    int L = sqrt(pow(lastPoint[0], 2) + pow(lastPoint[1], 2)) / distance;
    float teta = acos(lastPoint[0] / distance);
    //int N = pow(L, 2) * complexity;
    //float delta = 2 * M_PI / N;
    //float alpha = teta + delta;
    //float beta = teta - delta;
    //TODO: get the points of lower level inside the angle
    int n = static_cast<int>(pow(L - 1, 2) * complexity);
    int a = static_cast<int>(n * teta / (2 * M_PI) - pow(1 - 1 / L, 2));
    int b = static_cast<int>(n * teta / (2 * M_PI) + pow(1 - 1 / L, 2));
    for (int i = a; i <= b; i++){
        double angle = i * 2 * M_PI / n;
        int pt[] = {int(L * distance * cos(angle)), int(L * distance * sin(angle))};
        adjacent.push_back(pt);
    }
    //TODO: get the 2 adjacent points in the same level
    //TODO: get the next level points in angle
    return adjacent;
}

/*bool goodPoint(int* point[2]){
    return true;
}*/

void brain(int* lastPoint[2]){
    bool clockwise;
    //Finds the next point

    //continues from here
    std::vector<int[2]> adjacent = getAdjacent(*lastPoint); //, missed); //order: anticlockwize lower level, anticlockwi level, anticlockwise level, next level
    //TODO: case > has adjacent lower level unmapped point
    for (std::vector<int[2]>::iterator point = adjacent.begin() ; point != adjacent.end(); point++) {
        if (true){ //goodPoint(*point)) {
            break;
        } else {
            //expand search in the radios of the current point
            //TODO: translate central point to current point
            //TODO: find the next point with the previous algorithm
            //TODO:translate the center to the initial one and continue the search

        }
    }
    //TODO: check if unmaped points are surrounded by maped ones
    //if so, it eliminate them from the uncked list
}

int main (){
    return 0;
}
