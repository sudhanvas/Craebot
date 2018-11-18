#include <chrono>
#include <iostream>
#include <thread>

#include <zcm/zcm-cpp.hpp>

#include "common/timestamp.hh"
#include "common/transport.hh"
#include "zcmtypes/craebot/ascii_t.hpp"

using namespace std::chrono_literals;  // NOLINT

int main()
{
  zcm::ZCM zcm{craebot::common::transport::ZCM_UDPM_URL};

  if (!zcm.good())
  {
    std::cerr << "Failed to initialize ZCM." << std::endl;
    return EXIT_FAILURE;
  }

  craebot::ascii_t ascii_msg{};
  ascii_msg.nCharacters = 4;
  ascii_msg.data.resize(4);
  ascii_msg.data[0] = ' ';
  ascii_msg.data[1] = ' ';
  ascii_msg.data[2] = ' ';
  ascii_msg.data[3] = ' ';

  // Turn off all the four ASCII character displays.
  ascii_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(2s);

  // Print ASCII: "1   "
  ascii_msg.nCharacters = 1;
  ascii_msg.data[0] = '1';
  ascii_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(2s);

  // Print ASCII: "12  "
  ascii_msg.nCharacters = 2;
  ascii_msg.data[1] = '2';
  ascii_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(2s);

  // Print ASCII: "123 "
  ascii_msg.nCharacters = 3;
  ascii_msg.data[2] = '3';
  ascii_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(2s);

  // Print ASCII: "1234"
  ascii_msg.nCharacters = 4;
  ascii_msg.data[3] = '4';
  ascii_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(2s);

  // Turn off all the four ASCII character displays.
  ascii_msg.nCharacters = 4;
  ascii_msg.data[0] = ' ';
  ascii_msg.data[1] = ' ';
  ascii_msg.data[2] = ' ';
  ascii_msg.data[3] = ' ';
  ascii_msg.utime = craebot::common::time::now();
  zcm.publish("CRAEBOT_ASCII", &ascii_msg);
  std::this_thread::sleep_for(2s);

  return EXIT_SUCCESS;
}
