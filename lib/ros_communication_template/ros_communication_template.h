#ifndef ROS_COMMUNICATION_TEMPLATE_H
#define ROS_COMMUNICATION_TEMPLATE_H

// Generic starting point for a micro-ROS node: WiFi transport, one example
// subscriber, one example publisher on a timer. Copy this folder, rename
// the class/file, then work through every "TODO" below to wire in your
// own message types, topics and logic. See ros_communication/ in this same
// lib/ folder for a filled-in example (cmd_vel subscriber + /scan publisher).

#include <micro_ros_platformio.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

// TODO: swap for the message type(s) you actually need, e.g.:
//   #include <geometry_msgs/msg/twist.h>
//   #include <sensor_msgs/msg/laser_scan.h>
#include <std_msgs/msg/int32.h>

// How often the example publisher fires. Rename/adjust per topic if you
// end up with more than one timer-driven publisher.
#define EXAMPLE_PUBLISH_PERIOD_MS 1000

class RosCommunicationTemplate{
    public:
        RosCommunicationTemplate();
        void initialize();
        void subscriber_define();
        void publisher_define();
        void executors_start();
        void start_receiving_msgs();

        // TODO: rename to match your subscriber's topic/purpose
        static void example_subscriber_callback(const void *msg_recv);
        // TODO: rename to match your publisher's topic/purpose
        static void example_timer_callback(rcl_timer_t *timer, int64_t last_call_time);
};

#endif
