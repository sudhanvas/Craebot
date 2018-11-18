#include <cinttypes>
#include <iostream>
#include <string>

#include <boost/noncopyable.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/transport.hh"
#include "zcmtypes/craebot/bumper_t.hpp"

inline std::string boolalpha(int8_t boolean)
{
  return boolean == 1 ? "true" : "false";
}

class Handler : private boost::noncopyable
{
 public:
  Handler() = default;
  ~Handler() = default;

  void callback(const zcm::ReceiveBuffer * /*rbuf*/, const std::string & /*channel*/, const craebot::bumper_t *bumper)
  {
    std::cout << "utime:                     " << bumper->utime << std::endl;
    std::cout << "is_left_pressed:           " << boolalpha(bumper->is_left_pressed) << std::endl;
    std::cout << "is_right_pressed:          " << boolalpha(bumper->is_right_pressed) << std::endl;

    std::cout << "is_light_left:             " << boolalpha(bumper->is_light_left) << std::endl;
    std::cout << "is_light_front_left:       " << boolalpha(bumper->is_light_front_left) << std::endl;
    std::cout << "is_light_center_left:      " << boolalpha(bumper->is_light_center_left) << std::endl;
    std::cout << "is_light_center_right:     " << boolalpha(bumper->is_light_center_right) << std::endl;
    std::cout << "is_light_front_right:      " << boolalpha(bumper->is_light_front_right) << std::endl;
    std::cout << "is_light_right:            " << boolalpha(bumper->is_light_right) << std::endl;

    std::cout << "light_signal_left:         " << bumper->light_signal_left << std::endl;
    std::cout << "light_signal_front_left:   " << bumper->light_signal_front_left << std::endl;
    std::cout << "light_signal_center_left:  " << bumper->light_signal_center_left << std::endl;
    std::cout << "light_signal_center_right: " << bumper->light_signal_center_right << std::endl;
    std::cout << "light_signal_front_right:  " << bumper->light_signal_front_right << std::endl;
    std::cout << "light_signal_right:        " << bumper->light_signal_right << std::endl;

    std::cout << "omni_ir: " << bumper->omni_ir << std::endl;
  }
};

int main()
{
  zcm::ZCM zcm{craebot::common::transport::ZCM_UDPM_URL};

  if (!zcm.good())
  {
    std::cerr << "Failed to initialize ZCM\n";
    return EXIT_FAILURE;
  }

  Handler handler;
  zcm.subscribe("CRAEBOT_BUMPER", &Handler::callback, &handler);
  std::cout << "Subscribed to channel: CRAEBOT_BUMPER" << std::endl;
  zcm.run();

  return EXIT_SUCCESS;
}
