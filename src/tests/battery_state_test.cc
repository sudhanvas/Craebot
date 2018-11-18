#include <iostream>

#include <boost/noncopyable.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/transport.hh"
#include "zcmtypes/craebot/battery_state_t.hpp"

class Handler : private boost::noncopyable
{
 public:
  Handler() = default;
  ~Handler() = default;

  void callback(const zcm::ReceiveBuffer * /*rbuf*/,
                const std::string & /*channel*/,
                const craebot::battery_state_t *battery)
  {
    std::cout << "utime:          " << battery->utime << std::endl;
    std::cout << "voltage:        " << battery->voltage << std::endl;
    std::cout << "current:        " << battery->current << std::endl;
    std::cout << "charge:         " << battery->charge << std::endl;
    std::cout << "capacity:       " << battery->capacity << std::endl;
    std::cout << "ratio:          " << battery->charge_ratio << std::endl;
    std::cout << "temperature:    " << battery->temperature << std::endl;
    std::cout << "charging_state: " << battery->charging_state << std::endl;
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
  zcm.subscribe("CRAEBOT_BATTERY_STATE", &Handler::callback, &handler);
  std::cout << "Subscribed to channel: CRAEBOT_BATTERY_STATE" << std::endl;

  zcm.run();
  return EXIT_SUCCESS;
}
