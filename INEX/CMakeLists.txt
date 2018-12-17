cmake_minimum_required(VERSION 2.8.3)
project(inex)
find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
  std_msgs
)
catkin_package()
include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(inex src/inex.cpp)
target_link_libraries(inex
  ${catkin_LIBRARIES})

add_executable(image_node src/save_image.cpp)
 target_link_libraries(image_node
   ${catkin_LIBRARIES}
   ${OpenCV_LIBS}
)

add_executable(color_detection src/color_detection.cpp)
 target_link_libraries(color_detection
   ${catkin_LIBRARIES}
   ${OpenCV_LIBS}
)
