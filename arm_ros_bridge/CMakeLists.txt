cmake_minimum_required(VERSION 3.8)
project(arm_ros_bridge)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)

include_directories(
  include
  ../external/FTServo_Linux/src
)
add_executable(
  trajectory_bridge
  src/trajectory_bridge.cpp
  src/servo_mapper.cpp
  src/ftservo_driver.cpp
)

ament_target_dependencies(
  trajectory_bridge
  rclcpp
  moveit_msgs
)

target_link_libraries(
  trajectory_bridge
  ${CMAKE_CURRENT_SOURCE_DIR}/../external/FTServo_Linux/src/libSCServo.a
)

install(
  TARGETS trajectory_bridge
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
