#include <ros/ros.h>

// Recall that nodes subscribe to input topics and publish to output topics
// Topics contain messages, which have specific formatting rules
// This node will subscribe to keyboard inputs from the user and publish corresponding drive commands
// Keyboard inputs use the std_msgs::String format
// Drive commands use the ackermann_msgs::AckermannDriveStamped format
// You should type std_msgs::String and ackermann_msgs::AckermannDriveStamped into Google to see how the formatting works!
#include <std_msgs/String.h>
#include <ackermann_msgs/AckermannDriveStamped.h>
#include <ackermann_msgs/AckermannDrive.h>

class ManualControlNode {
private:
    // Create a ROS node object. The name can be anything.
    ros::NodeHandle n;
    // Create a Subscriber object. The name can be anything.
    ros::Subscriber key_sub;
    // Create a Publisher object. The name can be anything.
    ros::Publisher drive_cmd_pub;

public:
    ManualControlNode() { // Think of this as the node's constructor. It is the first thing that runs when you create a ManualControlNode object
        // Initialize the node
        n = ros::NodeHandle("~");

        // Get topic names from the params.yaml file
        // Using a separate file for defining parameters is useful for keeping all our parameter definitions in one place 
        std::string drive_topic, key_topic;
        n.getParam("mux_topic", drive_topic); // It's called mux_topic because the f1tenth simulator uses a mux to toggle between manual and autonomous control
        n.getParam("keyboard_topic", key_topic);

        // Set up the publisher. This is where you specify the topic of the publisher. The message format corresponds to the drive command format.
        drive_cmd_pub = n.advertise<ackermann_msgs::AckermannDriveStamped>(drive_topic, 10);

        // Set up the subscriber. This is where you specify the topic of the subscriber.
        // This is also where you specify the name of the function that will run every time a message is received on the subscribed topic. Here it is called key_callback, which is a naming convention.
        key_sub = n.subscribe(key_topic, 1, &ManualControlNode::key_callback, this);
    }


    void key_callback(const std_msgs::String & msg) {
        // make drive message from keyboard if turned on 
        if (mux_controller[key_mux_idx]) {
            // Determine desired velocity and steering angle
            double desired_velocity = 0.0;
            double desired_steer = 0.0;
            
            bool publish = true;

            if (msg.data == "w") {
                // Forward
                desired_velocity = keyboard_speed; // a good speed for keyboard control
            } else if (msg.data == "s") {
                // Backwards
                desired_velocity = -keyboard_speed;
            } else if (msg.data == "a") {
                // Steer left and keep speed
                desired_steer = keyboard_steer_ang;
                desired_velocity = prev_key_velocity;
            } else if (msg.data == "d") {
                // Steer right and keep speed
                desired_steer = -keyboard_steer_ang;
                desired_velocity = prev_key_velocity;
            } else if (msg.data == " ") {
                // publish zeros to slow down/straighten out car
            } else {
                // so that it doesn't constantly publish zeros when you press other keys
                publish = false;
            }

            if (publish) {
                publish_to_drive(desired_velocity, desired_steer);
                prev_key_velocity = desired_velocity;
            }
        }
    }

}; // end of class definition


int main(int argc, char ** argv) {
    ros::init(argc, argv, "node");
    // Create a ManualControlNode object. It can be named anything.
    ManualControlNode heyo;
    ros::spin();
    return 0;
}
