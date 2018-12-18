cmake_minimum_required(VERSION 2.8.3)
project(remote)
find_package(catkin REQUIRED roscpp)
catkin_package()
include_directories(
  ${catkin_INCLUDE_DIRS}
)
add_executable(remote src/remote.cpp)
target_link_libraries(remote
  ${catkin_LIBRARIES}
)
