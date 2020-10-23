#include "geometry_msgs/TwistStamped.h"
#include "jog_msgs/JogJoint.h"
#include "ros/ros.h"
#include "sensor_msgs/Joy.h"

// X button
#define ENABLE_LINEAR_BUTTON 1
// circle button
#define ENABLE_ANGULAR_BUTTON 2

// re-use same axes with different enable button for angular!
#define LINEAR_X_AXIS 1
#define LINEAR_Y_AXIS 0
#define LINEAR_Z_AXIS 7

namespace to_twist
{
class xboxToTwist
{
public:
  xboxToTwist() : spinner_(1)
  {
    joy_sub_ = n_.subscribe("joy", 1, &xboxToTwist::joyCallback, this);
    twist_pub_ = n_.advertise<geometry_msgs::TwistStamped>("jog_arm_server/delta_jog_cmds", 1);
    joint_delta_pub_ = n_.advertise<jog_msgs::JogJoint>("jog_arm_server/joint_delta_jog_cmds", 1);

    spinner_.start();
    ros::waitForShutdown();
  };

private:
  ros::NodeHandle n_;
  ros::Subscriber joy_sub_;
  ros::Publisher twist_pub_, joint_delta_pub_;
  ros::AsyncSpinner spinner_;

  // Convert incoming joy commands to TwistStamped commands for jogging
  void joyCallback(const sensor_msgs::Joy::ConstPtr& msg)
  {
    // Cartesian jogging
    geometry_msgs::TwistStamped twist;
    twist.header.stamp = ros::Time::now();

    // check if we're in angular or linear mode
    if (msg->buttons[ENABLE_LINEAR_BUTTON]) {
      twist.twist.linear.x = msg->axes[LINEAR_X_AXIS];
      twist.twist.linear.y = msg->axes[LINEAR_Y_AXIS];
      twist.twist.linear.z = msg->axes[LINEAR_Z_AXIS];

      twist.twist.angular.x = 0;
      twist.twist.angular.y = 0;
      twist.twist.angular.z = 0;
    }
    else if (msg->buttons[ENABLE_ANGULAR_BUTTON]) {
      twist.twist.linear.x = 0;
      twist.twist.linear.y = 0;
      twist.twist.linear.z = 0;

      twist.twist.angular.x = msg->axes[LINEAR_X_AXIS];
      twist.twist.angular.y = msg->axes[LINEAR_Y_AXIS];
      twist.twist.angular.z = msg->axes[LINEAR_Z_AXIS];
    }
    else {
      twist.twist.linear.x = 0;
      twist.twist.linear.y = 0;
      twist.twist.linear.z = 0;

      twist.twist.angular.x = 0;
      twist.twist.angular.y = 0;
      twist.twist.angular.z = 0;
    }

    joint_deltas.header.stamp = ros::Time::now();

    twist_pub_.publish(twist);
    joint_delta_pub_.publish(joint_deltas);
  }
};
}  // end to_twist namespace

int main(int argc, char** argv)
{
  ros::init(argc, argv, "ps4_to_twist");

  to_twist::xboxToTwist to_twist;

  return 0;
}
