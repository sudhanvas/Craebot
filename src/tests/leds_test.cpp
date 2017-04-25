#include <chrono>
#include <thread>

#include <lcm/lcm-cpp.hpp>

#include <common/timestamp.hpp>
#include <lcmtypes/craebot/leds_t.hpp>

using sec = std::chrono::seconds;

int main(void) {
  lcm::LCM lcm;
  if (!lcm.good()) {
    fprintf(stderr, "Failed to initialize LCM.\n");
    return EXIT_FAILURE;
  }

  craebot::leds_t leds_msg;
  leds_msg.utime = craebot::common::now();
  leds_msg.check = false;
  leds_msg.debris = false;
  leds_msg.dock = false;
  leds_msg.spot = false;
  leds_msg.power[0] = 0;
  leds_msg.power[1] = 0;

  //  Turn of all the LEDs.
  lcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(sec(2));

  // Turn on the Check LED for 2 seconds.
  leds_msg.check = true;
  leds_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(sec(2));

  // Turn on the Debris LED for 2 seconds.
  leds_msg.check = false;
  leds_msg.debris = true;
  leds_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(sec(2));

  // Turn on the Dock LED for 2 seconds.
  leds_msg.debris = false;
  leds_msg.dock = true;
  leds_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(sec(2));

  // Turn on the Spot LED for 2 seconds.
  leds_msg.dock = false;
  leds_msg.spot = true;
  leds_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(sec(2));

  // Turn on the Power LED: red/full intensity.
  leds_msg.spot = false;
  leds_msg.power[0] = 255;
  leds_msg.power[1] = 255;
  leds_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(sec(2));

  // Turn off all the LEDs before exiting.
  leds_msg.power[0] = 0;
  leds_msg.power[1] = 0;
  leds_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_LEDS", &leds_msg);
  std::this_thread::sleep_for(sec(2));

  return EXIT_SUCCESS;
}
