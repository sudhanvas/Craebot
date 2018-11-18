#ifndef CRAEBOT_DRIVER_HH_
#define CRAEBOT_DRIVER_HH_

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

#include <boost/noncopyable.hpp>
#include <create/create.h>
#include <zcm/zcm-cpp.hpp>

#include "common/timer_killer.hh"
#include "common/timestamp.hh"
#include "common/transport.hh"
#include "zcmtypes/craebot/ascii_t.hpp"
#include "zcmtypes/craebot/battery_state_t.hpp"
#include "zcmtypes/craebot/bumper_t.hpp"
#include "zcmtypes/craebot/buttons_t.hpp"
#include "zcmtypes/craebot/leds_t.hpp"
#include "zcmtypes/craebot/motor_feedback_t.hpp"
#include "zcmtypes/craebot/odometry_t.hpp"
#include "zcmtypes/craebot/teleop_t.hpp"

namespace craebot
{
namespace driver
{
class CraebotDriver : private boost::noncopyable
{
 public:
  CraebotDriver() = delete;
  explicit CraebotDriver(create::RobotModel robot_model,
                         std::unique_ptr<create::Create> robot_uptr,
                         const double &sensor_data_frequency,
                         const double &battery_state_frequency);
  ~CraebotDriver();

  void run();

 private:
  zcm::ZCM _m_zcm_udpm;
  create::RobotModel _m_robot_model;
  std::unique_ptr<create::Create> _m_robot_uptr;

  float _m_wheel_radius;
  std::chrono::milliseconds _m_sensor_data_sleep_ms;
  std::chrono::milliseconds _m_battery_state_sleep_ms;
  common::threading::TimerKiller _m_publish_threads_tk;
  std::thread _m_publish_sensor_data_thread;
  std::thread _m_publish_battery_state_thread;
  std::mutex _m_robot_mutex;

  battery_state_t _m_battery_state_msg;
  motor_feedback_t _m_motor_feedback_msg;
  bumper_t _m_bumper_msg;
  odometry_t _m_odometry_msg;
  buttons_t _m_buttons_msg;

  void __teleop_callback(const zcm::ReceiveBuffer *rbuf, const std::string &channel, const teleop_t *teleop);
  void __leds_callback(const zcm::ReceiveBuffer *rbuf, const std::string &channel, const leds_t *leds);
  void __ascii_callback(const zcm::ReceiveBuffer *rbuf, const std::string &channel, const ascii_t *ascii);

  void __publish_sensor_data_thread();
  void __publish_battery_state_thread();
  void __publish_motor_feedback();
  void __publish_bumper_data();
  void __publish_odometry_data();
  void __publish_button_states();

  void __init_msgs();
  void __init_robot();
  void __init_publish_frequencies(const double &sensor_data_frequency, const double &battery_state_frequency);
};

}  // namespace driver
}  // namespace craebot

#endif  // CRAEBOT_DRIVER_HH_
