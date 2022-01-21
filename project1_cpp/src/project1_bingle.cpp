#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include "project1_msg/msg/rvd.hpp"


using namespace std::chrono_literals;
using std::placeholders::_1;

class Project1PubAndSub : public rclcpp::Node
{
public:
  Project1PubAndSub()
  : Node("project1_bingler"), count_(0)
  {
    auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10));
    
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "/turtle1/cmd_vel", qos_profile);
    
    subscriber_ = this->create_subscription<project1_msg::msg::RVD>(
      "/topic", qos_profile,
      std::bind(&Project1PubAndSub::topic_callback, this, _1));
   
//    timer_ = this->create_wall_timer(
//      1s, std::bind(&Project1Publisher::timer_callback, this));
  }

private:
  void topic_callback(const project1_msg::msg::RVD::SharedPtr msg) const
  {
    
    bool d = msg->d;

    auto a = geometry_msgs::msg::Twist();

    auto lin_vec = geometry_msgs::msg::Vector3();
    if(d){
      lin_vec.x = msg->v;
    }
    else{
      lin_vec.x = -msg->v;
    }
    lin_vec.y = 0.0;
    lin_vec.z = 0.0;

    auto radi = msg->r;

    auto ang_vec = geometry_msgs::msg::Vector3();
    ang_vec.x = 0.0;
    ang_vec.y = 0.0;
    ang_vec.z = lin_vec.x / radi;

    a.linear = lin_vec;
    a.angular = ang_vec;
    
    RCLCPP_INFO(this->get_logger(), "Published linear vel: '%f'm/s", lin_vec.x);
    RCLCPP_INFO(this->get_logger(), "Published angular vel: '%f'rad/s", ang_vec.z);
    publisher_->publish(a);
  }
//  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<project1_msg::msg::RVD>::SharedPtr subscriber_;
  size_t count_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<Project1PubAndSub>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}