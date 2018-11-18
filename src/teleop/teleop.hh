#ifndef CRAEBOT_TELEOP_HH_
#define CRAEBOT_TELEOP_HH_

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

#include <boost/noncopyable.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/timer_killer.hh"
#include "common/timestamp.hh"
#include "common/transport.hh"
#include "zcmtypes/craebot/joystick_t.hpp"
#include "zcmtypes/craebot/teleop_t.hpp"

namespace craebot
{
namespace teleop
{
using ms = std::chrono::milliseconds;

struct app_params_t
{
  uint8_t linear_axis;
  uint8_t angular_axis;

  float linear_scale;
  float angular_scale;

  double publish_frequency;
};

class CraebotTeleop : private boost::noncopyable
{
 private:
  zcm::ZCM _m_zcm_udpm;

  uint8_t _m_linear_axis;
  uint8_t _m_angular_axis;
  float _m_linear_scale;
  float _m_angular_scale;

  teleop_t _m_teleop_msg;
  std::thread _m_publish_thread;
  std::mutex _m_teleop_mutex;
  common::threading::TimerKiller _m_publish_thread_tk;

  ms _m_publish_duration;

 public:
  CraebotTeleop() = delete;
  explicit CraebotTeleop(const app_params_t &params);
  ~CraebotTeleop();

  void run();

 private:
  void __callback(const zcm::ReceiveBuffer *rbuf, const std::string &channel, const joystick_t *joystick);
  void __publish_thread();
};

}  // namespace teleop
}  // namespace craebot

#endif  // CRAEBOT_TELEOP_HH_
