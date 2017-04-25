#ifndef __CRAEBOT_JOYSTICK_HPP__
#define __CRAEBOT_JOYSTICK_HPP__

#include <fcntl.h>
#include <linux/joystick.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <csignal>
#include <iostream>
#include <limits>
#include <thread>
#include <vector>

#include <lcm/lcm-cpp.hpp>

#include "common/timestamp.hpp"
#include "lcmtypes/craebot/joystick_t.hpp"

namespace craebot {
namespace sensors {

static const float DEFAULT_DEADZONE = 0.2;
static const float DEFAULT_AUTO_REPEAT_RATE = 0.00;
static const float DEFAULT_COALESCE_INTERVAL = 0.01;
static const std::string DEFAULT_DEVICE_FILENAME = "/dev/input/js2";

typedef struct {
  float deadzone;
  float auto_repeat_rate;
  float coalesce_interval;
  std::string device_filename;
} app_params_t;

class Joystick {
 private:
  lcm::LCM _m_lcm;

  float _m_deadzone;
  float _m_auto_repeat_rate;
  float _m_coalesce_interval;
  std::string _m_device_filename;

  struct sigaction _m_signal_action;
  static sig_atomic_t _m_quit;

 public:
  explicit Joystick(app_params_t& params);
  ~Joystick() { }

  void run();

 private:
  void _init_signals(void);
  static inline void _signal_handler(int signal);
};

} // namespace sensors
} // namespace craebot


#endif /* __CRAEBOT_JOYSTICK_HPP__ */
