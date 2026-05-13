#include "arm_ros_bridge/ftservo_driver.hpp"

#include <iostream>

FTServoDriver::FTServoDriver()
{
}

bool FTServoDriver::init(
    const char* device,
    int baudrate)
{
    return sms_sts_.begin(
        baudrate,
        device
    );
}

void FTServoDriver::writePosition(
    int servo_id,
    int pulse,
    int speed,
    int accel)
{
    sms_sts_.WritePosEx(
        servo_id,
        pulse,
        speed,
        accel
    );
}