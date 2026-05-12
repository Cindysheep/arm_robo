# URDF for HOPEJr Arm

## Envirenment
* Ubuntu 22.04
* ROS2 Humble

## Display
Run the follow command in the ros2 workspace
```bash
cd ~/ws_moveit
colcon build
source install/setup.bash
ros2 launch Arm_urdf display.launch.py
```
