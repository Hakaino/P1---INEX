#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>
#include <nav_msgs/Odometry.h>

bool anomaly = false;

double x_pose;
double y_pose;
char filename[80];

class ImageConverter{
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
   cv::Mat hsv_img, mask_img;
   cvtColor(cv_ptr->image, hsv_img, cv::COLOR_RGB2HSV);
   cv::inRange(hsv_img, cv::Scalar(0, 255, 50), cv::Scalar(179, 255, 90), mask_img);

   cv::erode(mask_img, mask_img, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
   cv::dilate(mask_img, mask_img, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

   cv::dilate(mask_img, mask_img, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
   cv::erode(mask_img, mask_img, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

   std::vector<std::vector<cv::Point> > contours;
   std::vector<cv::Vec4i> hierarchy;
   cv::RNG rng(12345);
   cv::findContours(mask_img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

   cv::Mat drawing = cv::Mat::zeros(mask_img.size(), CV_8UC3);
   for( int i = 0; i< contours.size(); i++ ){
    cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
    cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
  }

  int largest_area = 9500; //Change values to adjust search size
  int lowest_area = 15000;

  for(int i = 0; i< contours.size(); i++ ){
    double area = contourArea(contours[i],false);
    if(area>largest_area && area<lowest_area){
      sprintf(filename,"/home/dud/ros_ws/src/INEX/anomalies/x:%f_y:%f.png", x_pose, y_pose);
      cv::imwrite(filename,cv_ptr->image);
      anomaly = true;
    }
  }
   image_pub_.publish(cv_ptr->toImageMsg());
 }
};

ros::Subscriber odom_sub_;

void clbk_asd(const nav_msgs::Odometry::ConstPtr& asd){
  x_pose = asd->pose.pose.position.x;
  y_pose = asd->pose.pose.position.y;
  if(anomaly){
    ROS_INFO("Anomaly detected at: x: %f, y: %f",x_pose, y_pose);
    anomaly = false;
    return;
  }
}

int main(int argc, char** argv){
 ros::init(argc, argv, "color_detection");
 ros::NodeHandle n;
 ImageConverter ic;
 ros::spinOnce();
 odom_sub_ = n.subscribe("/odom", 1, clbk_asd);
 ros::spin();
 return 0;
}
