#include <chrono>
#include <iostream>
#include <thread>

#include <zcm/zcm-cpp.hpp>

#include "common/timestamp.hh"
#include "common/transport.hh"
#include "zcmtypes/craebot/leds_t.hpp"

using namespace std::chrono_literals;  // NOLINT

int main()
{
  zcm::ZCM zcm{craebot::common::transport::ZCM_UDPM_URL};

  if (!zcm.good())
  {
    std::cerr << "Failed to initialize ZCM\n";
    return EXIT_FAILURE;
  }

  craebot::leds_t leds_msg;
  leds_msg.utime = craebot::common::time::now();
  leds_msg.check = 0;
  leds_msg.debris = 0;
  leds_msg.dock = 0;
  leds_msg.spot = 0;
  leds_msg.power[0] = 0;
  leds_msg.power[1] = 0;

  // Turn of all the LEDs.
  zcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(2s);

  // Turn on the Check LED for 2 seconds.
  leds_msg.check = 1;
  leds_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(2s);

  // Turn on the Debris LED for 2 seconds.
  leds_msg.check = 0;
  leds_msg.debris = 1;
  leds_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(2s);

  // Turn on the Dock LED for 2 seconds.
  leds_msg.debris = 0;
  leds_msg.dock = 1;
  leds_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(2s);

  // Turn on the Spot LED for 2 seconds.
  leds_msg.dock = 0;
  leds_msg.spot = 1;
  leds_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(2s);

  // Turn on the Power LED: red/full intensity.
  leds_msg.spot = 0;
  leds_msg.power[0] = 255;
  leds_msg.power[1] = 255;
  leds_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(2s);

  // Turn off all the LEDs before exiting.
  leds_msg.power[0] = 0;
  leds_msg.power[1] = 0;
  leds_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(2s);

  return EXIT_SUCCESS;
}
