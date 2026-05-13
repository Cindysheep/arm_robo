#pragma once

#include <string>
#include <unordered_map>

struct JointConfig
{
    int servo_id;

    double offset_rad;

    int direction;

    int min_pulse;
    int max_pulse;
};

static std::unordered_map<std::string, JointConfig>
JOINT_MAP =
{
    {
        "Joint_1",
        {
            7,
            -2.584757760185942,
            -1,
            0,
            4095
        }
    },

    {
        "Joint_2",
        {
            6,
            1.0385049933985793,
            1,
            0,
            4095
        }
    },

    {
        "Joint_3",
        {
            5,
            -1.2972817273391380,
            -1,
            0,
            4095
        }
    },

    {
        "Joint_4",
        {
            1,
            2.0033789089786474, // (742 - 2048) * PI / 2048 / -1  => positive
            -1,
            0,
            4095
        }
    },

    {
        "Joint_5",
        {
            2,
            -1.9957090050392193, // (747 - 2048) * PI / 2048 / 1
            1,
            0,
            4095
        }
    },

    {
        "Joint_6",
        {
            3,
            3.0664275949833968, // (4047 - 2048) * PI / 2048 / 1
            1,
            0,
            4095
        }
    },

    {
        "Joint_7",
        {
            7,          // servo_id (assumed)
            0.0,        // offset_rad (no measurement for ID 7)
            1,  
            0,
            4095
        }
    }
};