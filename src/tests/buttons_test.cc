#include <iostream>
#include <string>

#include <boost/noncopyable.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/transport.hh"
#include "zcmtypes/craebot/buttons_t.hpp"

inline std::string boolalpha(int8_t boolean)
{
  return boolean == 1 ? "true" : "false";
}

class Handler : private boost::noncopyable
{
 public:
  Handler() = default;
  ~Handler() = default;

  void callback(const zcm::ReceiveBuffer* /*rbuf*/, const std::string& /*channel*/, const craebot::buttons_t* buttons)
  {
    std::cout << "utime:     " << buttons->utime << std::endl;
    std::cout << "clean:     " << boolalpha(buttons->clean) << std::endl;
    std::cout << "cliff:     " << boolalpha(buttons->cliff) << std::endl;
    std::cout << "dock:      " << boolalpha(buttons->dock) << std::endl;
    std::cout << "day:       " << boolalpha(buttons->day) << std::endl;
    std::cout << "hour:      " << boolalpha(buttons->hour) << std::endl;
    std::cout << "minute:    " << boolalpha(buttons->minute) << std::endl;
    std::cout << "spot:      " << boolalpha(buttons->spot) << std::endl;
    std::cout << "wheeldrop: " << boolalpha(buttons->wheeldrop) << std::endl;
    std::cout << std::endl;
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
  zcm.subscribe("CRAEBOT_BUTTONS", &Handler::callback, &handler);
  std::cout << "Subscribed to channel: CRAEBOT_BUTTONS" << std::endl;
  zcm.run();

  return EXIT_SUCCESS;
}
