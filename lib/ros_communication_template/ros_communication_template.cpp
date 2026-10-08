#include "ros_communication_template.h"

rcl_subscription_t example_sub;
std_msgs__msg__Int32 example_sub_msg;

rcl_publisher_t example_pub;
std_msgs__msg__Int32 example_pub_msg;
rcl_timer_t example_timer;

// Prefixed to avoid clashing with the identically-named globals in
// ros_communication.cpp if this template is ever compiled alongside it.
rclc_executor_t tpl_executor;
rclc_support_t tpl_support;
rcl_allocator_t tpl_allocator;
rcl_node_t tpl_node;

// Fail hard on setup errors (node/publisher/subscriber/timer creation) so a
// broken config is obvious instead of silently half-working.
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) { error_loop(); } }
// Ignore errors from calls that can harmlessly fail per-iteration (e.g. publish
// while the agent is briefly unreachable).
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) {} }

static void error_loop() {
    while (1) {
        delay(100);
    }
}

RosCommunicationTemplate::RosCommunicationTemplate(){
}

void RosCommunicationTemplate::initialize(){
    Serial.begin(115200);
    Serial.println("ROS Communication node started");

    // TODO: replace with your own network + agent details, or swap this
    // block for set_microros_serial_transports(Serial) if you'd rather run
    // over USB serial instead of WiFi (see code/microROS/src/main.cpp for
    // the serial-transport version of this same setup).
    IPAddress agent_ip(192, 168, 0, 152);
    size_t agent_port = 8888;
    char ssid[] = "YOUR_WIFI_SSID";
    char psk[] = "YOUR_WIFI_PASSWORD";
    set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);

    delay(2000);
    if (rmw_uros_ping_agent(100, 5) == RMW_RET_OK) {
        Serial.println("Agent found");
    } else {
        Serial.println("Agent not found - check IP/port and that the agent is running");
    }

    tpl_allocator = rcl_get_default_allocator();
    RCCHECK(rclc_support_init(&tpl_support, 0, NULL, &tpl_allocator));
    // TODO: give the node a name that describes this robot/board
    RCCHECK(rclc_node_init_default(&tpl_node, "template_node", "", &tpl_support));
}

void RosCommunicationTemplate::subscriber_define(){
    // TODO: point at your real message type/topic, e.g.
    //   ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "/cmd_vel"
    RCCHECK(rclc_subscription_init_default(
        &example_sub,
        &tpl_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "/example_sub"));
}

void RosCommunicationTemplate::example_subscriber_callback(const void *msg_recv){
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *) msg_recv;
    Serial.print("Received: ");
    Serial.println(msg->data);

    // TODO: act on the message here (drive motors, update state, etc.)
}

void RosCommunicationTemplate::publisher_define(){
    // TODO: point at your real message type/topic, e.g.
    //   ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, LaserScan), "/scan"
    // Any message with dynamic (sequence/string) fields needs its backing
    // storage pointed at static memory here too - see ros_communication.cpp's
    // publisher_define() for a worked example with sensor_msgs/LaserScan.
    RCCHECK(rclc_publisher_init_default(
        &example_pub,
        &tpl_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "/example_pub"));

    RCCHECK(rclc_timer_init_default(
        &example_timer,
        &tpl_support,
        RCL_MS_TO_NS(EXAMPLE_PUBLISH_PERIOD_MS),
        RosCommunicationTemplate::example_timer_callback));
}

void RosCommunicationTemplate::example_timer_callback(rcl_timer_t *timer, int64_t last_call_time){
    (void) last_call_time;
    if (timer == NULL) return;

    // TODO: fill example_pub_msg with real data before publishing
    example_pub_msg.data++;
    RCSOFTCHECK(rcl_publish(&example_pub, &example_pub_msg, NULL));
}

void RosCommunicationTemplate::executors_start(){
    // Handle count must match the number of add_subscription/add_timer
    // calls below - bump it whenever you add another one.
    RCCHECK(rclc_executor_init(&tpl_executor, &tpl_support.context, 2, &tpl_allocator));
    RCCHECK(rclc_executor_add_subscription(
        &tpl_executor, &example_sub, &example_sub_msg,
        &RosCommunicationTemplate::example_subscriber_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_timer(&tpl_executor, &example_timer));
    Serial.println("Executors Started");
}

void RosCommunicationTemplate::start_receiving_msgs(){
    rclc_executor_spin_some(&tpl_executor, RCL_MS_TO_NS(100));
    delay(100);
}
