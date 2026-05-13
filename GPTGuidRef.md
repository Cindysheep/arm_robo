你现在已经完成了整个系统里最关键的 60%：

* MoveIt 能成功对 URDF 做运动规划
* `plan & execute` 已经能输出完整轨迹
* 官方 SDK 已经能控制真实电机
* 你已经写了 `trajectory_bridge`
* MoveIt 输出已经能拿到：

  * joint name
  * position(rad)
  * velocity
  * time_from_start

这意味着：

> 你现在缺的不是“运动学”，而是：
>
> “把 MoveIt 的轨迹实时、安全、稳定地翻译成 STS/SMS 电机协议并执行”。

你接下来应该进入：

# 第二阶段：真实机械臂控制系统

整个系统建议按下面顺序做。

---

# 总体规划（推荐顺序）

# Phase 1：建立“单关节闭环”

目标：

MoveIt 输出一个 joint angle →
真实电机准确运动 →
并能读取当前位置

你要完成：

1. 完善 `servo_mapper`
2. 建立 joint ↔ servo id 映射
3. rad ↔ pulse 转换
4. 发送单电机位置命令
5. 读取单电机反馈
6. 校准零点

这是整个系统最关键的一步。

---

# Phase 2：实现多关节同步控制

目标：

MoveIt 输出 trajectory →
多个舵机同步运动

你要完成：

1. JointTrajectory 遍历
2. 多电机同步写入
3. time_from_start 插值执行
4. 控制频率设计
5. 轨迹缓冲

这一阶段结束后：

你的人形机械臂已经真正“动起来”。

---

# Phase 3：建立 ROS2 Control Hardware Interface（强烈推荐）

目标：

让 MoveIt 认为：
“真实机械臂就是 ros2_control 的标准硬件”。

这是正式工程化方案。

你要完成：

1. 编写 hardware_interface plugin
2. export_state_interfaces
3. export_command_interfaces
4. read()
5. write()

完成后：

MoveIt → ros2_control → 真实机械臂

整个链路正规化。

---

# Phase 4：真实闭环控制

目标：

不是“发送目标位置”，
而是：

实时读取编码器 →
反馈 →
控制

你要完成：

1. read encoder
2. current position update
3. JointState 发布
4. RViz 实时同步
5. PID/速度限制
6. 误差检测

---

# Phase 5：高级功能

包括：

* 碰撞检测
* 自碰撞
* 速度规划
* 笛卡尔控制
* 逆运动学优化
* 力控
* teleop
* imitation learning
* RL
* 人形上半身控制

---

# 下面详细讲每一步

---

# PHASE 1

# Step 1：建立 joint → servo id 映射

你现在 MoveIt 里是：

```txt
Joint_1
Joint_2
...
Joint_7
```

真实世界是：

```txt
ID=1
ID=2
...
```

你需要：

# servo_mapper.hpp

```cpp
#pragma once

#include <string>
#include <unordered_map>

struct ServoConfig
{
    int id;

    double rad_min;
    double rad_max;

    int pulse_min;
    int pulse_max;

    bool reversed;
};

class ServoMapper
{
public:

    static const ServoConfig& getConfig(
        const std::string& joint_name
    );

    static int radToPulse(
        const std::string& joint_name,
        double rad
    );

    static double pulseToRad(
        const std::string& joint_name,
        int pulse
    );
};
```

---

# 为什么必须这样？

因为：

不同关节：

* 零点不同
* 方向不同
* 行程不同
* 编码器范围不同

不能简单：

```cpp
pulse = rad * K
```

否则机械臂一定会：

* 方向反
* 撞限位
* 姿态错乱

---

# Step 2：建立标定参数

你必须做：

# 每个关节记录：

| joint | servo_id | rad_min | rad_max | pulse_min | pulse_max | reversed |
| ----- | -------- | ------- | ------- | --------- | --------- | -------- |

例如：

```cpp
{
    "Joint_1",
    {
        1,
        -1.57,
        1.57,
        1000,
        3000,
        false
    }
}
```

---

# Step 3：实现 radToPulse

核心公式：

```cpp
ratio = (rad - rad_min) / (rad_max - rad_min)

pulse = pulse_min + ratio * (pulse_max - pulse_min)
```

反向：

```cpp
if(reversed)
    ratio = 1.0 - ratio;
```

最后 clamp。

---

# Step 4：测试单关节

不要直接上七轴。

先：

# 独立测试一个电机

例如：

```cpp
Joint_3
```

目标：

```txt
0 rad
0.3 rad
-0.3 rad
```

观察：

* 方向是否正确
* 零点是否正确
* 是否超限位
* 是否抖动

---

# Step 5：建立真实零点

这是最容易炸机械臂的地方。

你必须：

# 手动定义：

MoveIt 的：

```txt
0 rad
```

对应真实机械臂什么姿态。

通常：

* Home pose
* 所有关节居中

然后记录：

```txt
该姿态下的 pulse
```

作为 offset。

最终公式：

```cpp
pulse = offset + mapped_value
```

---

# 推荐结构

# servo_mapper.cpp

```cpp
static std::unordered_map<std::string, ServoConfig> configs
```

里面保存：

* id
* offset
* range
* reverse

---

# PHASE 2

# Step 6：真正执行 trajectory

你现在已经拿到了：

```txt
Point 24
Joint_1 pos
Joint_2 pos
...
```

下一步：

# 对每个 point：

```cpp
for point in trajectory.points
```

转换：

```cpp
rad -> pulse
```

然后：

```cpp
SyncWritePosEx(...)
```

发送。

---

# Step 7：使用同步写入（极其重要）

不要：

```cpp
WritePosEx(id1)
WritePosEx(id2)
WritePosEx(id3)
```

因为：

会导致：

* 不同步
* 机械臂抽搐
* 姿态变形

必须：

# FTServo SDK 的 SyncWrite

所有舵机同时开始运动。

---

# Step 8：time_from_start

MoveIt 给你的：

```txt
time_from_start = 2.379
```

不是装饰。

这是：

# “这个点应该在什么时候到达”

你必须：

# 方法1（推荐）

计算：

```cpp
dt = next_time - current_time
```

然后：

```cpp
sleep_for(dt)
```

---

# Step 9：速度控制

很多 FTServo：

```cpp
WritePosEx(id, pos, speed, acc)
```

MoveIt 已经给了：

```txt
vel: xxx
```

你可以：

```cpp
speed = rad/s -> servo_speed
```

否则：

先固定速度也行。

---

# PHASE 3（非常重要）

# ros2_control 硬件接口

你现在是：

```txt
MoveIt
  ↓
trajectory_bridge
  ↓
舵机
```

这是能工作的。

但后期会很难维护。

标准方案应该是：

```txt
MoveIt
  ↓
ros2_control
  ↓
HardwareInterface
  ↓
FTServo
```

---

# 为什么必须做？

因为：

MoveIt 官方默认就是：

```txt
FollowJointTrajectory
```

控制器。

而 ros2_control：

* 自动管理 JointState
* 自动同步 RViz
* 自动 controller manager
* 支持 fake hardware
* 支持 Gazebo
* 支持实时控制

---

# 你最终应该写：

# FTServoHardware

继承：

```cpp
hardware_interface::SystemInterface
```

实现：

---

# export_state_interfaces()

告诉 ROS：

当前关节状态来自哪里。

---

# export_command_interfaces()

告诉 ROS：

目标位置写到哪里。

---

# read()

从舵机读取：

```txt
当前位置
速度
电流
```

---

# write()

发送：

```txt
目标位置
```

---

# PHASE 4

# Step 10：发布真实 JointState

现在 RViz 里：

MoveIt 在“假装机械臂运动”。

未来应该：

真实编码器 →
JointState →
RViz

这样：

真实机械臂和 RViz 完全同步。

---

# Step 11：安全系统（非常重要）

必须做：

# 1. 软件限位

防止：

```txt
超过关节极限
```

---

# 2. 急停

例如：

```txt
CTRL+C
```

或：

```txt
emergency stop
```

---

# 3. 速度限制

尤其人形机械臂。

---

# 4. 碰撞检测

MoveIt 已经支持。

---

# 你现在最应该先做的

按顺序：

# 第一优先级

## 完善 servo_mapper

这是核心。

---

# 第二优先级

## 单电机闭环测试

验证：

```txt
rad -> pulse -> motor
```

正确。

---

# 第三优先级

## trajectory_bridge 真正发送多电机同步命令

---

# 第四优先级

## 建立 ros2_control hardware interface

---

# 你目前的系统结构建议

建议改成：

```txt
arm_ros_bridge
├── include/arm_ros_bridge
│   ├── servo_mapper.hpp
│   ├── ftservo_driver.hpp
│   ├── hardware_interface.hpp
│   └── trajectory_bridge.hpp
│
├── src
│   ├── servo_mapper.cpp
│   ├── ftservo_driver.cpp
│   ├── hardware_interface.cpp
│   └── trajectory_bridge.cpp
```

---

# 你下一步最具体应该做什么（今天）

# TODAY TODO

# 1

完成：

```cpp
ServoConfig
```

---

# 2

完成：

```cpp
radToPulse()
pulseToRad()
```

---

# 3

建立：

```cpp
joint -> servo id
```

表

---

# 4

写：

```cpp
move_single_joint.cpp
```

测试：

```txt
0 rad
0.2 rad
-0.2 rad
```

---

# 5

确认：

* 方向
* 零点
* 限位
* 编码器读数

全部正确。

---

# 最后一个非常关键的建议

不要一开始：

```txt
MoveIt -> 七轴真机
```

直接跑。

这是非常危险的。

正确流程：

# 单轴

→ 双轴
→ 三轴
→ 全轴

否则：

* 撞限位
* 电流过大
* 齿轮损坏
* 连杆打架

非常容易出现。
