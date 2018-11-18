#include <iostream>
#include <memory>
#include <utility>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "lib/factory.hh"
#include "lib/joystick.hh"
#include "lib/service.hh"

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

class Args
{
 public:
  craebot::sensors::joystick::app_params_t params{};
  std::string device_filename{};

 public:
  bool parse(int argc, char *argv[])
  {
    bpo::options_description description{
        "ZCM driver for a generic Linux joystick. This app publishes messages containing "
        "the current state of each one of the joystick's buttons and axes"};
    description.add_options()("help,h", "Show this help text and exit.")(
        "deadzone,d",
        bpo::value<double>()->default_value(0.2),
        "Amount by which the joystick has to move before it is considered to be off-center.")(
        "auto-repeat-rate,a",
        bpo::value<double>()->default_value(0.0),
        "Rate in Hz at which a joystick that has a non-changing state will resend the previously sent message.")(
        "coalesce-interval,c",
        bpo::value<double>()->default_value(0.01),
        "This option can be used to limit the rate of outgoing messages.")(
        "sticky-buttons,s", bpo::bool_switch(), "Enables toggling the state of the button on every button press.")(
        "filename,f", bpo::value<std::string>()->default_value("/dev/input/js1"), "The joystick device filename.");

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, description), vm);
    bpo::notify(vm);

    if (vm.count("help") != 0u)
    {
      std::cout << description << std::endl;
      return true;
    }

    try
    {
      params.deadzone = vm["deadzone"].as<double>();
      params.auto_repeat_rate = vm["auto-repeat-rate"].as<double>();
      params.coalesce_interval = vm["coalesce-interval"].as<double>();
      params.sticky_buttons = vm["sticky-buttons"].as<bool>();
      device_filename = vm["filename"].as<std::string>();
    }
    catch (const bpo::error &err)
    {
      std::cerr << "Invalid Argument: " << err.what() << std::endl;
      return false;
    }

    if (!bfs::exists(device_filename))
    {
      std::cerr << "The device " << device_filename << " does not exist." << std::endl;
      return false;
    }

    return true;
  }
};

int main(int argc, char *argv[])
{
  Args args;
  if (!args.parse(argc, argv))
  {
    return EXIT_FAILURE;
  }

  try
  {
    auto joystick_uptr = craebot::sensors::joystick::factory::create_concrete_joystick(args.device_filename);
    craebot::sensors::joystick::JoystickService service{std::move(joystick_uptr), args.params};

    service.init();
    service.run();
  }
  catch (const std::exception &x)
  {
    std::cerr << "CraebotJoystick: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
