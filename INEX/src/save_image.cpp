#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

int i = 0;
char filename[50];

class ImageConverter
{
 ros::NodeHandle nh_;
 image_transport::ImageTransport it_;
 image_transport::Subscriber image_sub_;
 image_transport::Publisher image_pub_;

public:
 ImageConverter()
   : it_(nh_){
   image_sub_ = it_.subscribe("/camera/rgb/image_raw", 1, &ImageConverter::imageCb, this);
   image_pub_ = it_.advertise("/camera/image_processed", 1);
 }

 void imageCb(const sensor_msgs::ImageConstPtr& msg){
   cv_bridge::CvImagePtr cv_ptr;
   try{
     cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
   }
   catch (cv_bridge::Exception& e){
     ROS_ERROR("cv_bridge exception: %s", e.what());
     return;
   }
   sprintf(filename,"/home/dud/ros_ws/src/INEX/photos/photo_%d.png", i++);
     cv::imwrite(filename,cv_ptr->image);
     ros::Duration(10).sleep();
 }
};

int main(int argc, char** argv){
 ros::init(argc, argv, "image_node");
 ImageConverter ic;
 ros::spin();
 return 0;
}
