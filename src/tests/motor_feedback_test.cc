#include <iostream>

#include <boost/noncopyable.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/transport.hh"
#include "zcmtypes/craebot/motor_feedback_t.hpp"

class Handler : private boost::noncopyable
{
 public:
  Handler() = default;
  ~Handler() = default;

  void callback(const zcm::ReceiveBuffer * /*rbuf*/,
                const std::string & /*channel*/,
                const craebot::motor_feedback_t *feedback)
  {
    std::cout << "utime:                " << feedback->utime << std::endl;
    std::cout << "left_wheel_position:  " << feedback->left_wheel_position << std::endl;
    std::cout << "right_wheel_position: " << feedback->right_wheel_position << std::endl;
    std::cout << "left_wheel_speed:     " << feedback->left_wheel_speed << std::endl;
    std::cout << "right_wheel_speed:    " << feedback->right_wheel_speed << std::endl;
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
  zcm.subscribe("CRAEBOT_MOTOR_FEEDBACK", &Handler::callback, &handler);
  std::cout << "Subscribed to channel: CRAEBOT_MOTOR_FEEDBACK" << std::endl;
  zcm.run();

  return EXIT_SUCCESS;
}
