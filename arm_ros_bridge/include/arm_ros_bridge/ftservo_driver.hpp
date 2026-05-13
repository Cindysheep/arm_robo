#pragma once

#include "SCServo.h"

class FTServoDriver
{
public:

    FTServoDriver();

    bool init(
        const char* device,
        int baudrate
    );

    void writePosition(
        int servo_id,
        int pulse,
        int speed = 2400,
        int accel = 50
    );

private:

    SMS_STS sms_sts_;
};