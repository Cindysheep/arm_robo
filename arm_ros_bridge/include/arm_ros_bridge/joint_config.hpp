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
            1,
            0.0,
            1,
            0,
            4095
        }
    },

    {
        "Joint_2",
        {
            2,
            0.0,
            1,
            0,
            4095
        }
    },

    {
        "Joint_3",
        {
            3,
            0.0,
            1,
            0,
            4095
        }
    },

    {
        "Joint_4",
        {
            4,
            0.0,
            1,
            0,
            4095
        }
    },

    {
        "Joint_5",
        {
            5,
            0.0,
            1,
            0,
            4095
        }
    },

    {
        "Joint_6",
        {
            6,
            0.0,
            1,
            0,
            4095
        }
    },

    {
        "Joint_7",
        {
            7,
            0.0,
            1,
            0,
            4095
        }
    }
};