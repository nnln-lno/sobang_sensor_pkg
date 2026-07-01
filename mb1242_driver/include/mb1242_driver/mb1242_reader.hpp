#ifndef MB1242_READER_HPP_
#define MB1242_READER_HPP_

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"

#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS     "/dev/i2c-2"
#define MB1242_ADDR 0x70

namespace mb1242_reader
{
    class MB1242Reader : public rclcpp::Node
    {
        public:
            MB1242Reader();

            ~MB1242Reader();

            int fd_;

            void parseSonarData();
            
            sensor_msgs::msg::Range sonar_data_;

            sensor_msgs::msg::Range getSonarData() const { return sonar_data_; }

            rclcpp::Publisher<sensor_msgs::msg::Range>::SharedPtr sonar_pub_;

        private:
            
            rclcpp::TimerBase::SharedPtr timer_;
    };
}

#endif // MB1242_READER_HPP_