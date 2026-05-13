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

double ServoMapper::pulseToRad(
    const std::string& joint_name,
    int pulse)
{
    auto cfg = JOINT_MAP[joint_name];

    // ===== limit =====
    pulse = std::clamp(pulse, cfg.min_pulse, cfg.max_pulse);

    // ===== pulse → rad'（带方向和offset的中间值） =====
    double rad_signed =
        (static_cast<double>(pulse) - 2048.0) * M_PI / 2048.0;

    // ===== 反向方向变换 & 反向 offset =====
    if (std::abs(cfg.direction) < 1e-9) {
        // 防止除以 0，异常情况返回 0
        return 0.0;
    }
    double rad = rad_signed / cfg.direction;
    rad -= cfg.offset_rad;

    return rad;
}
