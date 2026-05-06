#include <gazebo/gazebo_client.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/poses_stamped.pb.h>

#include <ignition/math/Quaternion.hh>
#include <iostream>

void cb(ConstPosesStampedPtr &_msg)
{
    for (int i = 0; i < _msg->pose_size(); i++)
    {
        const auto &p = _msg->pose(i);

        // ✅ filter only base model
        if (p.name() != "robot_model")
            continue;

        // Position
        double x = p.position().x();
        double y = p.position().y();
        double z = p.position().z();

        // Orientation (quaternion)
        double qx = p.orientation().x();
        double qy = p.orientation().y();
        double qz = p.orientation().z();
        double qw = p.orientation().w();

        // Convert to Euler (roll, pitch, yaw)
        ignition::math::Quaterniond q(qw, qx, qy, qz);
        auto euler = q.Euler();  // roll, pitch, yaw

        std::cout << "\n=== ROBOT BASE ===\n";

        std::cout << "Position:\n";
        std::cout << "  x: " << x
                  << " y: " << y
                  << " z: " << z << "\n";

        std::cout << "Quaternion:\n";
        std::cout << "  x: " << qx
                  << " y: " << qy
                  << " z: " << qz
                  << " w: " << qw << "\n";

        std::cout << "Euler (rpy):\n";
        std::cout << "  roll: "  << euler.X()
                  << " pitch: " << euler.Y()
                  << " yaw: "   << euler.Z() << "\n";
    }
}
int main()
{
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
        gazebo::common::Time::MSleep(100);
    }

    gazebo::client::shutdown();
}