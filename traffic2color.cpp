#include "ros/ros.h"
#include "opencv2/opencv.hpp"
#include "darknet_ros/darknet_ros_msgs/msg/ObjectCount.h"
#include "darknet_ros/darknet_ros_msgs/msg/BoundingBox.h"

float box_num = 0;
float box_xmin[];
float box_ymin[];
float box_xmax[];
float box_ymax[];
cv_bridge::CvImage pub_image;

void BoxNumCallback(const darknet_ros_msgs::ObjectCount num){
    box_num << int num.count ;
    cout << int num.count << endl;
}

void BoxPointCallback(const darknet_ros_msgs::BoundingBoxes box){
    int i;
    for(i=0; i < box_num; i++){
        box_xmin[i] << box.xmin;
        box_ymin[i] << box.ymin;
        box_xmax[i] << box.xmax;
        box_ymax[i] << box.ymax;
    }
}

void ImageCropCallback(const sensor_msgs::ImageConstPtr& image){
    float weidth = (box_xmax - box_xmin);
    float height = (box_ymax - box_ymin);
    cv::Mat cam_image = cv_bridge::toCvCopy(image, "bgr8");
    cv::Mat crop_image = cv::Mat (cam_image, cv::Rect(box_xmin, box_ymin, width, height));
    pub_image = cv_bridge::toImageMsg(crop_image, "bgr8");
}

int main(int argc, char **argv){
    ros::init (argc, argv, "traffic2colour");
    ros::NodeHandle nh("~");

    ros::Subscriber sub_box_num = nh.subscribe("/darknet_ros/found_object", 2, BoxNumCallback);
    ros::Subscriber sub_box = nh.subscribe("/darknet_ros/bounding_boxes", 2, BoxPointCallback);
    ros::Subscriber sub_image = nh.subscribe("/camera/rgb/image_raw", 2, ImageCropCallback);

    ros::Publisher pub_image = nh.advertise<darknet_ros_msgs::CropImage>("croped_image", 2);
    ros::spin();
    return 0;
}