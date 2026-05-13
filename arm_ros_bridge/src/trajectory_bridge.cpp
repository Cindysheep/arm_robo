#include <rclcpp/rclcpp.hpp>

#include <moveit_msgs/msg/display_trajectory.hpp>

class TrajectoryBridge : public rclcpp::Node
{
public:

    TrajectoryBridge()
    : Node("trajectory_bridge")
    {
        subscription_ =
            this->create_subscription<
                moveit_msgs::msg::DisplayTrajectory>(
                    "/display_planned_path",
                    10,
                    std::bind(
                        &TrajectoryBridge::callback,
                        this,
                        std::placeholders::_1
                    )
                );

        RCLCPP_INFO(
            this->get_logger(),
            "Trajectory Bridge Started"
        );
    }

private:

    void callback(
        const moveit_msgs::msg::DisplayTrajectory::SharedPtr msg)
    {
        auto traj =
            msg->trajectory[0].joint_trajectory;

        auto names =
            traj.joint_names;

        RCLCPP_INFO(
            this->get_logger(),
            "Received trajectory with %ld points",
            traj.points.size()
        );

        rclcpp::Time start_time = this->now();

        for(size_t i = 0; i < traj.points.size(); i++)
        {
            auto point = traj.points[i];

            double target_time =
                point.time_from_start.sec +
                point.time_from_start.nanosec * 1e-9;

            // ===== 等待到对应时间 =====
            while(
                (this->now() - start_time).seconds()
                < target_time
            )
            {
                rclcpp::sleep_for(
                    std::chrono::milliseconds(1)
                );
            }

            RCLCPP_INFO(
                this->get_logger(),
                "========== Point %ld ==========",
                i
            );

            RCLCPP_INFO(
                this->get_logger(),
                "time_from_start = %.3f",
                target_time
            );

            for(size_t j = 0; j < names.size(); j++)
            {
                double pos =
                    point.positions[j];

                double vel = 0.0;

                if(j < point.velocities.size())
                {
                    vel =
                        point.velocities[j];
                }

                RCLCPP_INFO(
                    this->get_logger(),
                    "%s | pos: %.4f rad | vel: %.4f rad/s",
                    names[j].c_str(),
                    pos,
                    vel
                );

                // ===== 后面这里接 FTServo =====
            }
        }

    RCLCPP_INFO(
        this->get_logger(),
        "Trajectory execution finished"
    );
}

    rclcpp::Subscription<
        moveit_msgs::msg::DisplayTrajectory>::SharedPtr subscription_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    rclcpp::spin(
        std::make_shared<TrajectoryBridge>()
    );

    rclcpp::shutdown();

    return 0;
}