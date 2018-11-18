#include "teleop.hh"

namespace craebot
{
namespace teleop
{
CraebotTeleop::CraebotTeleop(const app_params_t &params)
    : _m_zcm_udpm{common::transport::ZCM_UDPM_URL},
      _m_linear_axis{params.linear_axis},
      _m_angular_axis{params.angular_axis},
      _m_linear_scale{params.linear_scale},
      _m_angular_scale{params.angular_scale}
{
  auto publish_duration_ms = static_cast<int64_t>(1.0 / params.publish_frequency * 1000.0);

  _m_publish_duration = ms(publish_duration_ms);

  _m_teleop_msg.utime = 0;
  _m_teleop_msg.angular_velocity = 0.f;
  _m_teleop_msg.linear_velocity = 0.f;

  if (!_m_zcm_udpm.good())
  {
    throw std::runtime_error("ZCM UDPM initialization failed.");
  }
}

CraebotTeleop::~CraebotTeleop()
{
  _m_publish_thread_tk.kill();
  _m_publish_thread.join();
  _m_zcm_udpm.stop();
}

void CraebotTeleop::run()
{
  _m_zcm_udpm.subscribe("CRAEBOT_JOYSTICK", &CraebotTeleop::__callback, this);
  _m_publish_thread = std::thread(&CraebotTeleop::__publish_thread, this);
  _m_zcm_udpm.start();
}

void CraebotTeleop::__callback(const zcm::ReceiveBuffer * /*rbuf*/, const std::string & /*channel*/,
                               const joystick_t *joystick)
{
  std::lock_guard<std::mutex> guard(_m_teleop_mutex);
  _m_teleop_msg.utime = craebot::common::time::now();
  _m_teleop_msg.angular_velocity = _m_angular_scale * joystick->axes[_m_angular_axis];
  _m_teleop_msg.linear_velocity = _m_linear_scale * joystick->axes[_m_linear_axis];
}

void CraebotTeleop::__publish_thread()
{
  while (_m_publish_thread_tk.wait_for(_m_publish_duration))
  {
    std::lock_guard<std::mutex> guard(_m_teleop_mutex);
    _m_zcm_udpm.publish("CRAEBOT_TELEOP", &_m_teleop_msg);
  }
}

}  // namespace teleop
}  // namespace craebot
