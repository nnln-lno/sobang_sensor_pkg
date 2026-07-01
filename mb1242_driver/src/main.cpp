#include <mb1242_driver/mb1242_reader.hpp>
#include <rclcpp/rclcpp.hpp>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<mb1242_reader::MB1242Reader>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}