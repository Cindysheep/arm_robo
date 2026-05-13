#pragma once

#include <string>

class ServoMapper
{
public:

    static int radToPulse(
        const std::string& joint_name,
        double rad
    );
};