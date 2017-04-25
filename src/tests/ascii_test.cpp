#include <chrono>
#include <thread>

#include <lcm/lcm-cpp.hpp>

#include <common/timestamp.hpp>
#include <lcmtypes/craebot/ascii_t.hpp>

using sec = std::chrono::seconds;

int main(void) {
  lcm::LCM lcm;
  if (!lcm.good()) {
    fprintf(stderr, "Failed to initialize LCM.\n");
    return EXIT_FAILURE;
  }

  craebot::ascii_t ascii_msg;
  ascii_msg.nCharacters = 4;
  ascii_msg.data.resize(4);
  ascii_msg.data[0] = ' ';
  ascii_msg.data[1] = ' ';
  ascii_msg.data[2] = ' ';
  ascii_msg.data[3] = ' ';

  // Turn off all the four ASCII character displays.
  ascii_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(sec(2));

  // Print ASCII: "1   "
  ascii_msg.nCharacters = 1;
  ascii_msg.data[0] = '1';
  ascii_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(sec(2));

  // Print ASCII: "12  "
  ascii_msg.nCharacters = 2;
  ascii_msg.data[1] = '2';
  ascii_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(sec(2));

  // Print ASCII: "123 "
  ascii_msg.nCharacters = 3;
  ascii_msg.data[2] = '3';
  ascii_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(sec(2));

  // Print ASCII: "1234"
  ascii_msg.nCharacters = 4;
  ascii_msg.data[3] = '4';
  ascii_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(sec(2));

  // Turn off all the four ASCII character displays.
  ascii_msg.nCharacters = 4;
  ascii_msg.data[0] = ' ';
  ascii_msg.data[1] = ' ';
  ascii_msg.data[2] = ' ';
  ascii_msg.data[3] = ' ';
  ascii_msg.utime = craebot::common::now();
  lcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(sec(2));

  return EXIT_SUCCESS;
}
