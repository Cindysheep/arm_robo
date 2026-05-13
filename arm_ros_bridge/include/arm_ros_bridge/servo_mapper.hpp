#pragma once

#include <string>

class ServoMapper
{
public:

    static int radToPulse(
        const std::string& joint_name,
        double rad
    );

    static double pulseToRad(
        const std::string& joint_name,
        int pulse
    );
};