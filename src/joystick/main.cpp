#include <getopt.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "joystick.hpp"

void usage(void) {
  printf("Usage: ./craebot-joystick [options]\n"
      "     LCM driver for a generic Linux joystick. This app publishes\n"
      "     messages containing the current state of each one of the\n"
      "     joystick's buttons and axes.\n"
      "\n"
      "Options:\n"
      "\n"
      "    -h, --help               Show this help text and exit.\n"
      "    -d, --dead-zone=VALUE    Amount by which the joystick has to move\n"
      "                             before it is considered to be off-center.\n"
      "                             [default: %.2f]\n"
      "    -a, --auto-repeat=VALUE  Rate in Hz at which a joystick that has\n"
      "                             a non-changing state will resend the\n"
      "                             previously sent message.\n"
      "                             [default: %.2f Hz]\n"
      "    -c, --coalesce=VALUE     Axis events that are received within\n"
      "                             VALUE (seconds) of each other are sent\n"
      "                             in a single ROS message.\n"
      "                             [default: %.2f s]\n"
      "    -f, --filename=NAME      The joystick device filename.\n"
      "                             [default: %s]\n",
      craebot::sensors::DEFAULT_DEADZONE,
      craebot::sensors::DEFAULT_AUTO_REPEAT_RATE,
      craebot::sensors::DEFAULT_COALESCE_INTERVAL,
      craebot::sensors::DEFAULT_DEVICE_FILENAME.c_str());
}

int main(int argc, char *argv[]) {
  setlinebuf(stdout);
  setlinebuf(stderr);

  craebot::sensors::app_params_t params;
  params.deadzone = craebot::sensors::DEFAULT_DEADZONE;
  params.auto_repeat_rate = craebot::sensors::DEFAULT_AUTO_REPEAT_RATE;
  params.coalesce_interval = craebot::sensors::DEFAULT_COALESCE_INTERVAL;
  params.device_filename = craebot::sensors::DEFAULT_DEVICE_FILENAME;

  const char* opt_string = "hd:a:c:f:l:";
  struct option long_opts[] = {
      {"help", no_argument, 0, 'h'},
      {"dead-zone", required_argument, 0, 'd'},
      {"auto-repeat", required_argument, 0, 'a'},
      {"coalesce", required_argument, 0, 'c'},
      {"filename", required_argument, 0, 'f'},
      {0, 0, 0, 0}};

  int c;
  while ((c = getopt_long(argc, argv, opt_string, long_opts, 0)) >= 0) {
    switch (c) {
      case 'h':
        usage();
        return EXIT_SUCCESS;
      case 'd':
        params.deadzone = static_cast<float> (atof(optarg));
        break;
      case 'a':
        params.auto_repeat_rate = static_cast<float> (atof(optarg));
        break;
      case 'c':
        params.coalesce_interval = static_cast<float> (atof(optarg));
        break;
      case 'f':
        params.device_filename = optarg;
        break;
      default:
        usage();
        return EXIT_FAILURE;
    }
  }

  craebot::sensors::Joystick joystick(params);
  joystick.run();

  return EXIT_SUCCESS;
}
