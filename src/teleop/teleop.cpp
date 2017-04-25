#include "teleop.hpp"

namespace craebot {
namespace teleop {

sig_atomic_t CraebotTeleop::_m_quit = false;
CraebotTeleop* CraebotTeleop::_m_this_instance = nullptr;

CraebotTeleop::CraebotTeleop(app_params_t &params)
    : _m_linear_axis(params.linear_axis)
    , _m_angular_axis(params.angular_axis)
    , _m_linear_scale(params.linear_scale)
    , _m_angular_scale(params.angular_scale)
    , _m_publish_thread(nullptr) {
  _m_this_instance = this;
  
  _init_signals();

  long publish_duration_ms =
      static_cast<long> (1.0/params.publish_frequency * 1000.0);
  _m_publish_duration = ms(publish_duration_ms);

  if (!_m_lcm.good()) {
    fprintf(stderr, "LCM initialization failed.\n");
    exit(EXIT_FAILURE);
  }

  _m_teleop_msg.utime = 0;
  _m_teleop_msg.linear_velocity = 0.0f;
  _m_teleop_msg.angular_velocity = 0.0f;

  _m_lcm.subscribe("CRAEBOT_JOYSTICK", &CraebotTeleop::_callback, this);
  _m_publish_thread = new std::thread(&CraebotTeleop::_publish_thread, this);

  while (!_m_lcm.handle());
}

CraebotTeleop::~CraebotTeleop() {
  _cleanup();
}

void CraebotTeleop::_init_signals(void) {
  _m_signal_action.sa_handler = _signal_handler;
  sigfillset(&_m_signal_action.sa_mask);
  _m_signal_action.sa_flags |= SA_SIGINFO;
  sigaction(SIGINT, &_m_signal_action, NULL);
}

void CraebotTeleop::_cleanup() {
  _m_publish_thread->join();
  delete _m_publish_thread;
}

void CraebotTeleop::_callback(
    const lcm::ReceiveBuffer* rbuf,
    const std::string& channel,
    const joystick_t* joystick) {
  std::lock_guard<std::mutex> guard(_m_teleop_mutex);

  _m_teleop_msg.utime = craebot::common::now();
  _m_teleop_msg.angular_velocity =
      _m_angular_scale * joystick->axes[_m_angular_axis];
  _m_teleop_msg.linear_velocity =
      _m_linear_scale * joystick->axes[_m_linear_axis];
}

void CraebotTeleop::_publish_thread(void) {
  while (!_m_quit) {
    _m_teleop_mutex.lock();
    _m_lcm.publish("CRAEBOT_TELEOP", &_m_teleop_msg);
    _m_teleop_mutex.unlock();
    std::this_thread::sleep_for(_m_publish_duration);
  }
}

inline void CraebotTeleop::_signal_handler(int signal) {
  if (signal == SIGINT) {
    printf("Goodbye!\n");
    _m_quit = true;
    _m_this_instance->_cleanup();
    std::exit (EXIT_SUCCESS);
  }
}


} // namespace teleop
} // namespace craebot
