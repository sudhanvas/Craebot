#ifndef __CRAEBOT_TELEOP_HPP__
#define __CRAEBOT_TELEOP_HPP__

#include <chrono>
#include <csignal>
#include <cstdint>
#include <mutex>
#include <thread>

#include <lcm/lcm-cpp.hpp>

#include "lcmtypes/craebot/joystick_t.hpp"
#include "lcmtypes/craebot/teleop_t.hpp"
#include "common/timestamp.hpp"

namespace craebot {
namespace teleop {

static const uint8_t DEFAULT_LINEAR_AXIS = 1;
static const uint8_t DEFAULT_ANGULAR_AXIS = 0;
static const float DEFAULT_LINEAR_SCALE = 0.5;
static const float DEFAULT_ANGULAR_SCALE = 4.25;
static const double DEFAULT_PUBLISH_FREQUENCY = 10.0;

using ms = std::chrono::milliseconds;

typedef struct {
  uint8_t linear_axis;
  uint8_t angular_axis;

  float linear_scale;
  float angular_scale;
  double publish_frequency;
} app_params_t;

class CraebotTeleop {
 private:
  lcm::LCM _m_lcm;

  uint8_t _m_linear_axis;
  uint8_t _m_angular_axis;
  float _m_linear_scale;
  float _m_angular_scale;

  teleop_t _m_teleop_msg;
  std::thread* _m_publish_thread;
  std::mutex _m_teleop_mutex;
  ms _m_publish_duration;

  static CraebotTeleop* _m_this_instance;
  struct sigaction _m_signal_action;
  static sig_atomic_t _m_quit;

 public:
  explicit CraebotTeleop(app_params_t &params);
  ~CraebotTeleop();

 private:
  void _init_signals(void);
  void _cleanup(void);

  void _callback(
      const lcm::ReceiveBuffer *rbuf,
      const std::string &channel,
      const joystick_t *joystick);
  void _publish_thread(void);

  static inline void _signal_handler(int signal);
};

} // namespace teleop
} // namespace craebot

#endif /* __CRAEBOT_TELEOP_HPP__ */
