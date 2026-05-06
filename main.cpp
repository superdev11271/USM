#include <gazebo/gazebo_client.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/poses_stamped.pb.h>

#include <geometry_msgs/msg/pose.hpp>
#include <rclcpp/rclcpp.hpp>
#include <iostream>

namespace
{
rclcpp::Publisher<geometry_msgs::msg::Pose>::SharedPtr g_pose_pub;
}

void cb(ConstPosesStampedPtr &_msg)
{
    for (int i = 0; i < _msg->pose_size(); i++)
    {
        const auto &p = _msg->pose(i);

        // ✅ filter only base model
        if (p.name() != "robot_model")
            continue;

        if (!g_pose_pub)
            continue;

        geometry_msgs::msg::Pose pose_msg;
        pose_msg.position.x = p.position().x();
        pose_msg.position.y = p.position().y();
        pose_msg.position.z = p.position().z();
        pose_msg.orientation.x = p.orientation().x();
        pose_msg.orientation.y = p.orientation().y();
        pose_msg.orientation.z = p.orientation().z();
        pose_msg.orientation.w = p.orientation().w();

        g_pose_pub->publish(pose_msg);
    }
}
int main()
{
    rclcpp::init(0, nullptr);
    auto ros_node = std::make_shared<rclcpp::Node>("gazebo_pose_bridge");
    g_pose_pub = ros_node->create_publisher<geometry_msgs::msg::Pose>("robot_pos", 10);

    // IMPORTANT: must connect to Gazebo master explicitly
    gazebo::client::setup();

    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // IMPORTANT: wait until connection is fully established
    gazebo::common::Time::MSleep(2000);

    // subscribe AFTER full connection
    auto sub = node->Subscribe("/gazebo/default/pose/info", cb);

    std::cout << "Subscribed OK\n";

    while (true)
    {
        rclcpp::spin_some(ros_node);
        gazebo::common::Time::MSleep(100);
    }

    gazebo::client::shutdown();
    rclcpp::shutdown();
}