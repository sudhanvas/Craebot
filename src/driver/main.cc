#include <atomic>
#include <csignal>
#include <functional>
#include <iostream>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "driver.hh"
#include "factory.hh"

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace cdf = craebot::driver::factory;

std::atomic<bool> stop{false};

void signal_handler(int /*signal*/)
{
  std::cout << "Goodbye!" << std::endl;
  stop.store(true);
}

class Args
{
 public:
  double sensor_data_frequency = 0.0;
  double battery_state_frequency = 0.0;

  std::string robot_model_name;
  std::string robot_device_filename;

  bool parse(int argc, char *argv[])
  {
    bpo::options_description description{
        "ZCM driver for the iRobot Create 2 Programmable robot. The driver listens "
        "to CRAEBOT_TELEOP and published sensor data to various channels"};
    description.add_options()("help,h", "Show this help text and exit.")(
        "model,m",
        bpo::value<std::string>()->default_value("create2"),
        R"(The model name of the robot. Could be one of the following: ["create1", "create2", "roomba400"].)")(
        "sensor-frequency,s", bpo::value<double>()->default_value(15.0), "Publish frequency of battery state.")(
        "battery-frequency,b", bpo::value<double>()->default_value(0.016667), "Publish frequency of battery state.")(
        "device,d",
        bpo::value<std::string>()->default_value("/dev/ttyUSB0"),
        "The serial device filename of the robot.");

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, description), vm);
    bpo::notify(vm);

    if (vm.count("help") != 0u)
    {
      std::cout << description << std::endl;
      return false;
    }

    try
    {
      sensor_data_frequency = vm["sensor-frequency"].as<double>();
      battery_state_frequency = vm["battery-frequency"].as<double>();

      robot_model_name = vm["model"].as<std::string>();
      robot_device_filename = vm["device"].as<std::string>();
    }
    catch (const bpo::error &err)
    {
      std::cerr << "Invalid Argument: " << err.what() << std::endl;
      return false;
    }

    if (!bfs::exists(robot_device_filename))
    {
      std::cerr << "The device " << robot_device_filename << " does not exist." << std::endl;
      return false;
    }

    return true;
  }
};

int main(int argc, char **argv)
{
  std::signal(SIGKILL, &signal_handler);
  std::signal(SIGINT, &signal_handler);
  std::signal(SIGTERM, &signal_handler);

  Args args;
  if (!args.parse(argc, argv))
  {
    return EXIT_FAILURE;
  }

  create::RobotModel robot_model{create::RobotModel::CREATE_2};
  std::unique_ptr<create::Create> robot_uptr;

  try
  {
    cdf::robot_t robot = cdf::create_concrete_robot(args.robot_model_name, args.robot_device_filename);
    robot_model = robot.first;
    robot_uptr = std::move(robot.second);
  }
  catch (std::exception &x)
  {
    std::cerr << "CraebotDriver:: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    craebot::driver::CraebotDriver driver{
        robot_model, std::move(robot_uptr), args.sensor_data_frequency, args.battery_state_frequency};
    driver.run();
    while (!stop)
    {
    }
  }
  catch (std::exception &x)
  {
    std::cerr << "CraebotDriver: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
