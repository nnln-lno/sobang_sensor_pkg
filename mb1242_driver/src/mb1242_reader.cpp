#include <mb1242_driver/mb1242_reader.hpp>

namespace mb1242_reader
{
    MB1242Reader::MB1242Reader() : Node("sonar_node")
    {
        sonar_pub_ = this->create_publisher<sensor_msgs::msg::Range>("sonar/range", 10);

        fd_ = open(I2C_BUS, O_RDWR);
        if (fd_ < 0)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to open I2C bus");
            rclcpp::shutdown();
            return;
        }

        if (ioctl(fd_, I2C_SLAVE, MB1242_ADDR) < 0)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to set I2C address");
            close(fd_);
            rclcpp::shutdown();
            return;
        }

        RCLCPP_INFO(this->get_logger(), "MB1242Reader node initialized");

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100),
            std::bind(&MB1242Reader::parseSonarData, this));
    }

    void MB1242Reader::parseSonarData()
    {
        uint8_t command = 0x51; // Command to initiate a ranging measurement
        if (write(fd_, &command, 1) != 1)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to write to I2C bus");
            return;
        }

        usleep(100000);

        uint8_t buf[2] = {0, 0};
        if (read(fd_, buf, 2) != 2)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to read from I2C bus");
            return;
        }

        uint16_t dist_cm = (buf[0] << 8) | buf[1];

        if (dist_cm < 20 || dist_cm > 765)
        {
            RCLCPP_WARN(this->get_logger(), "Invalid distance reading: %u cm", dist_cm);
            return;
        }

        sonar_data_.header.stamp = this->now();
        sonar_data_.header.frame_id = "sonar_frame";
        sonar_data_.radiation_type = sensor_msgs::msg::Range::ULTRASOUND;
        sonar_data_.field_of_view = 0.1745f; // Example value, adjust as needed
        sonar_data_.min_range = 0.2f; // 2 cm
        // 3.3V 기준으로 최대 거리는 사실상 500cm 입니다.
        sonar_data_.max_range = 7.65f; // 765 cm
        sonar_data_.range = static_cast<float>(dist_cm) / 100.0f;
        sonar_pub_->publish(sonar_data_);
    }

    MB1242Reader::~MB1242Reader()
    {
        if(fd_ >= 0) close(fd_);
    }
}
