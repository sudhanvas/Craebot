#include <getopt.h>

#include "teleop.hpp"

void usage(void) {
  printf("Usage: ./craebot-teleop [options]\n"
      "     LCM teleop node to control the iRobot Create 2. By default,\n"
      "     this program assumes the Xbox 360 Wireless Controller config.\n"
      "\n"
      "Options:\n"
      "    -h, --help                 Show this help text and exit.\n"
      "    -x, --linear-axis=INDEX    The index of the joystick axis to\n"
      "                               be used for the linear velocity.\n"
      "                               [default: %d]\n"
      "    -y, --angular-axis=INDEX   The index of the joystick axis to\n"
      "                               be used for the angular velocity.\n"
      "                               [default: %d]\n"
      "    -l, --linear-scale=VALUE   The scale with which the joystick value\n"
      "                               will be multiplied by before sending it\n"
      "                               as linear velocity to the robot.\n"
      "                               [default: %.2f]\n"
      "    -a, --angular-scale=VALUE  The scale with which the joystick value\n"
      "                               will be multiplied by before sending it\n"
      "                               as angular velocity to the robot.\n"
      "                               [default: %.2f]\n"
      "    -f, --frequency=VALUE      Rate in Hz at which a teleop that has\n"
      "                               a non-changing state will resend the\n"
      "                               previously sent message.\n"
      "                               [default: %.2f Hz]\n",
      craebot::teleop::DEFAULT_LINEAR_AXIS,
      craebot::teleop::DEFAULT_ANGULAR_AXIS,
      craebot::teleop::DEFAULT_LINEAR_SCALE,
      craebot::teleop::DEFAULT_ANGULAR_SCALE,
      craebot::teleop::DEFAULT_PUBLISH_FREQUENCY);
}

int main(int argc, char *argv[]) {
  setlinebuf(stdout);
  setlinebuf(stderr);

  craebot::teleop::app_params_t params;
  params.linear_axis = craebot::teleop::DEFAULT_LINEAR_AXIS;
  params.angular_axis = craebot::teleop::DEFAULT_ANGULAR_AXIS;
  params.linear_scale = craebot::teleop::DEFAULT_LINEAR_SCALE;
  params.angular_scale = craebot::teleop::DEFAULT_ANGULAR_SCALE;
  params.publish_frequency = craebot::teleop::DEFAULT_PUBLISH_FREQUENCY;

  const char* opt_string = "hx:y:l:a:j:t:";
  struct option long_opts[] = {
      {"help", no_argument, 0, 'h'},
      {"linear-axis", required_argument, 0, 'x'},
      {"angular-axis", required_argument, 0, 'y'},
      {"linear-scale", required_argument, 0, 'l'},
      {"angular-scale", required_argument, 0, 'a'},
      {"frequency", required_argument, 0, 'f'},
      {"joystick-channel", required_argument, 0, 'r'},
      {"teleop-channel", required_argument, 0, 't'}};

  int c;
  while ((c = getopt_long(argc, argv, opt_string, long_opts, 0)) >= 0) {
    switch (c) {
      case 'h':
        usage();
        break;
      case 'x':
        params.linear_axis = static_cast<uint8_t> (atoi(optarg));
        break;
      case 'y':
        params.angular_axis = static_cast<uint8_t> (atoi(optarg));
        break;
      case 'l':
        params.angular_scale = static_cast<float> (atof(optarg));
        break;
      case 'a':
        params.angular_scale = static_cast<float> (atof(optarg));
        break;
      case 'f':
        params.publish_frequency = static_cast<float> (atof(optarg));
        break;
      default:
        usage();
        return EXIT_FAILURE;
    }
  }

  craebot::teleop::CraebotTeleop teleop(params);

  return EXIT_SUCCESS;
}

