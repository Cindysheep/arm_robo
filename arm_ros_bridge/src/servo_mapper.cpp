#include "arm_ros_bridge/servo_mapper.hpp"

#include "arm_ros_bridge/joint_config.hpp"

#include <cmath>
#include <algorithm>

int ServoMapper::radToPulse(
    const std::string& joint_name,
    double rad)
{
    auto cfg =
        JOINT_MAP[joint_name];

    // ===== 加 offset =====
    rad += cfg.offset_rad;

    // ===== 方向 =====
    rad *= cfg.direction;

    // ===== rad → pulse =====
    int pulse =
        static_cast<int>(
            2048.0 +
            rad * 2048.0 / M_PI
        );

    // ===== limit =====
    pulse =
        std::clamp(
            pulse,
            cfg.min_pulse,
            cfg.max_pulse
        );

    return pulse;
}