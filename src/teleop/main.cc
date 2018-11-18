#include <iostream>

#include <boost/program_options.hpp>

#include "teleop.hh"

namespace bpo = boost::program_options;

volatile std::atomic_bool stop{false};

void signal_handler(int /*signal*/)
{
  std::cout << "Goodbye!" << std::endl;
  stop = true;
}

class Args
{
 public:
  craebot::teleop::app_params_t params{};

 public:
  bool parse(int argc, char *argv[])
  {
    uint8_t linear_axis{1};
    uint8_t angular_axis{0};

    bpo::options_description description{
        "ZCM teleop node to control the iRobot Create 2. By default, "
        "this program assumes the Xbox 360 Wireless Controller config"};
    description.add_options()("help,h", "Show this help text and exit.")(
        "linear-axis,x", bpo::value<uint8_t>(&linear_axis),
        "The index of the joystick axis to be used for the linear velocity.")(
        "angular-axis,y", bpo::value<uint8_t>(&angular_axis),
        "The index of the joystick axis to be used for the angular velocity.")(
        "linear-scale,l", bpo::value<float>()->default_value(0.5),
        "The scale with which the joystick value will be multiplied "
        "by before sending it as linear velocity to the robot.")(
        "angular-scale,a", bpo::value<float>()->default_value(4.25),
        "The scale with which the joystick value will be multiplied "
        "by before sending it as angular velocity to the robot.")(
        "frequency,f", bpo::value<double>()->default_value(10.0),
        "Rate in Hz at which a teleop that has a non-changing state will resend the previously sent message.");

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
      params.linear_axis = linear_axis;
      params.angular_axis = angular_axis;
      params.linear_scale = vm["linear-scale"].as<float>();
      params.angular_scale = vm["angular-scale"].as<float>();
      params.publish_frequency = vm["frequency"].as<double>();
    }
    catch (const bpo::error &err)
    {
      std::cerr << "Invalid Argument: " << err.what() << std::endl;
      return false;
    }

    return true;
  }
};

int main(int argc, char *argv[])
{
  std::signal(SIGKILL, &signal_handler);
  std::signal(SIGINT, &signal_handler);
  std::signal(SIGTERM, &signal_handler);

  Args args;
  if (!args.parse(argc, argv))
  {
    return EXIT_FAILURE;
  }

  try
  {
    craebot::teleop::CraebotTeleop teleop{args.params};
    teleop.run();
    while (!stop)
    {
    }
  }
  catch (const std::exception &x)
  {
    std::cerr << "CraebotTeleop: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
