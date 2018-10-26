cmake_minimum_required(VERSION 2.8.3)
project(INEX)
find_package(catkin REQUIRED roscpp turtlesim geometry_msgs)
catkin_package()
include_directories(
  ${catkin_INCLUDE_DIRS}
)
add_executable(INEX src/main.cpp)
target_link_libraries(INEX
  ${catkin_LIBRARIES}
)
