#include <getopt.h>

#include <cstdio>

#include "driver.hpp"

void usage(void) {
  printf("Usage: ./craebot-driver [options]\n"
      "     LCM driver for the iRobot Create 2 Programmable robot. The driver\n"
      "     listens to CRAEBOT_TELEOP and published sensor data to TODO.\n"
      "\n"
      "Options:\n"
      "    -h, --help            Show this help text and exit.\n"
      "    -m, --model=NAME      The model name of the robot. Could be one "
      "                          of the following: [\"create1\", \"create2\",\n"
      "                          \"roomba400\"]. [default: %s]\n"
      "    -s, --sensor-frequency=VALUE   Publish frequency of sensor data.\n"
      "                                   [default: %.2f Hz]\n"
      "    -b, --battery-frequency=VALUE  Publish frequency of battery state.\n"
      "                                   [default: %.5f Hz]\n"
      "    -d, --device=NAME     The serial device filename of the robot.\n"
      "                          [default: %s]\n",
      craebot::driver::DEFAULT_ROBOT_MODEL.c_str(),
      craebot::driver::DEFAULT_SENSOR_DATA_FREQUENCY,
      craebot::driver::DEFAULT_BATTERY_STATE_FREQUENCY,
      craebot::driver::DEFAULT_DEVICE_FILENAME.c_str());
}

int main(int argc, char *argv[]) {
  setlinebuf(stdout);
  setlinebuf(stderr);

  craebot::driver::app_params_t params;
  params.model_name = craebot::driver::DEFAULT_ROBOT_MODEL;
  params.sensor_data_frequency =
      craebot::driver::DEFAULT_SENSOR_DATA_FREQUENCY;
  params.battery_state_frequency =
      craebot::driver::DEFAULT_BATTERY_STATE_FREQUENCY;
  params.device_filename = craebot::driver::DEFAULT_DEVICE_FILENAME;

  const char* opt_string = "hm:f:d:b:";
  struct option long_opts[] = {
      {"help", no_argument, 0, 'h'},
      {"model", required_argument, 0, 'm'},
      {"sensors-frequency", required_argument, 0, 's'},
      {"battery-frequency", required_argument, 0, 'b'},
      {"device", required_argument, 0, 'd'}};

  int c;
  while ((c = getopt_long(argc, argv, opt_string, long_opts, 0)) >= 0) {
    switch (c) {
      case 'h':
        usage();
        exit (EXIT_SUCCESS);
      case 'm':
        params.model_name = optarg;
        break;
      case 's':
        params.sensor_data_frequency = atof(optarg);
        break;
      case 'b':
        params.battery_state_frequency = atof(optarg);
        break;
      case 'd':
        params.device_filename = optarg;
      default:
        usage();
        return EXIT_FAILURE;
    }
  }

  craebot::driver::CraebotDriver driver(params);
  driver.run();

  return EXIT_SUCCESS;
}
