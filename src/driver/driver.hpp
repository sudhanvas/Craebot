#ifndef __CRAEBOT_DRIVER_HPP__
#define __CRAEBOT_DRIVER_HPP__

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <thread>

#include <create/create.h>
#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/craebot/buttons_t.hpp>

#include "common/timestamp.hpp"
#include "lcmtypes/craebot/ascii_t.hpp"
#include "lcmtypes/craebot/battery_state_t.hpp"
#include "lcmtypes/craebot/bumper_t.hpp"
#include "lcmtypes/craebot/leds_t.hpp"
#include "lcmtypes/craebot/motor_feedback_t.hpp"
#include "lcmtypes/craebot/odometry_t.hpp"
#include "lcmtypes/craebot/teleop_t.hpp"

namespace craebot {
namespace driver {

typedef struct {
  std::string model_name;
  std::string device_filename;
  double sensor_data_frequency;
  double battery_state_frequency;
} app_params_t;

static const double DEFAULT_SENSOR_DATA_FREQUENCY = 15.0;
static const double DEFAULT_BATTERY_STATE_FREQUENCY = 0.016667;
static const std::string DEFAULT_ROBOT_MODEL = "create2";
static const std::string DEFAULT_DEVICE_FILENAME = "/dev/ttyUSB0";

class CraebotDriver {
 private:
  lcm::LCM _m_lcm;
  create::Create * _m_robot_ptr;
  create::RobotModel _m_robot_model;

  std::chrono::milliseconds _m_sensor_data_sleep_ms;
  std::chrono::milliseconds _m_battery_state_sleep_ms;
  std::thread* _m_publish_sensor_data_thread;
  std::thread* _m_publish_battery_state_thread;
  std::mutex _m_robot_mutex;

  battery_state_t _m_battery_state_msg;
  motor_feedback_t _m_motor_feedback_msg;
  bumper_t _m_bumper_msg;
  odometry_t _m_odometry_msg;
  buttons_t _m_buttons_msg;
  buttons_t _m_last_buttons_msg;

  // Needed for cleanup
  static CraebotDriver* _m_this_instance;
  struct sigaction _m_signal_action;
  static sig_atomic_t _m_quit;

 public:
  explicit CraebotDriver(app_params_t& params);
  ~CraebotDriver();
  void run(void);

 private:
  void _init_msg(void);

  void _teleop_callback(
      const lcm::ReceiveBuffer* rbuf,
      const std::string& channel,
      const teleop_t* teleop);
  void _leds_callback(
      const lcm::ReceiveBuffer* rbuf,
      const std::string& channel,
      const leds_t* leds);
  void _ascii_callback(
      const lcm::ReceiveBuffer* rbuf,
      const std::string& channel,
      const ascii_t* ascii);

  void _publish_sensor_data_thread(void);
  void _publish_battery_state_thread(void);
  void _publish_motor_feedback(void);
  void _publish_bumper_data(void);
  void _publish_odometry_data(void);
  void _publish_button_states(void);

  void _init_signals(void);
  void _cleanup(void);
  static inline void _signal_handler(int signal);
};

bool operator!= (const buttons_t& lhs, const buttons_t& rhs);

} // namespace driver
} // namespace craebot

#endif /* __CRAEBOT_DRIVER_HPP__ */
