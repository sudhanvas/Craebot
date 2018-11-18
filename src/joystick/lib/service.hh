#ifndef CRAEBOT_SENSORS_JOYSTICK_SERVICE_HH_
#define CRAEBOT_SENSORS_JOYSTICK_SERVICE_HH_

#include <algorithm>
#include <atomic>
#include <cmath>
#include <csignal>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <boost/noncopyable.hpp>
#include <boost/variant.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/timer_killer.hh"
#include "common/timestamp.hh"
#include "common/transport.hh"
#include "joystick.hh"
#include "zcmtypes/craebot/joystick_t.hpp"

namespace craebot
{
namespace sensors
{
namespace joystick
{
struct app_params_t
{
  double deadzone;
  double auto_repeat_rate;
  double coalesce_interval;
  bool sticky_buttons;
};

class JoystickService : private boost::noncopyable
{
 public:
  JoystickService() = delete;
  ~JoystickService();
  explicit JoystickService(std::unique_ptr<Joystick> joystick_uptr, const app_params_t &params);

  void init();
  void run();

 private:
  zcm::ZCM _m_zcm_udpm;
  std::unique_ptr<Joystick> _m_joystick_uptr;
  app_params_t _m_app_params;

  craebot::joystick_t _m_joy_msg;
  craebot::joystick_t _m_last_published_joy_msg;  // used for sticky buttons option
  craebot::joystick_t _m_sticky_buttons_joy_msg;  // used for sticky buttons option

  std::mutex _m_joy_msg_mutex;
  std::thread _m_publish_thread;
  craebot::common::threading::TimerKiller _m_publish_thread_tk;

  static std::atomic<bool> _m_stop_loop;

 private:
  void __publish_joystick_data_thread();
  void __init_signals() const;
  void __validate_app_params();

  static void _s_signal_handler(int signal);
};

}  // namespace joystick
}  // namespace sensors
}  // namespace craebot

#endif  // CRAEBOT_SENSORS_JOYSTICK_SERVICE_HH_
