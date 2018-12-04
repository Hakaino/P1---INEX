
#include <iostream>
#include <math.h>
#include <vector>

int distance = 10;
int complexity = 4;

std::vector<int> getAdjacent(int x, int y){ //, std::vector<int> missedPoints){
    int n, a, b;
    std::vector<int> adjacent, holder;
    double angle;
    //Characterize the current position
    int l = sqrt(pow(x, 2) + pow(y, 2)) / distance;
    int L = l;
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
            std::cout << "#";
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
            std::cout << "*";
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

/*bool goodPoint(int* point[2]){
    if (not point not in map) return true;
    else return false;
}*/

void brain(int* x, int* y){
    bool clockwise;
    //Finds the next point and sort them in order or priority
    //clockwise, lower level, current level, upper level
    std::vector<int> adjacent = getAdjacent(*x, *y);
    //TODO: check in order if a point can be a goal
    for (std::vector<int>::iterator point = adjacent.begin() ; point != adjacent.end(); point += 2) {
        if (true){ //goodPoint(*point, point + 1)) { //may have error
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
    //just testing
    std::vector<int> points = getAdjacent(2000, 20000);
    for (int i = 0; i < points.size(); i += 2)
        std::cout << points[i] << ", " << points[i + 1] << "\n";
    return 0;
}
