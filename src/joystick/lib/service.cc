#include "service.hh"

namespace craebot
{
namespace sensors
{
namespace joystick
{
std::atomic<bool> JoystickService::_m_stop_loop{false};

JoystickService::JoystickService(std::unique_ptr<Joystick> joystick_uptr, const app_params_t &params)
    : _m_zcm_udpm{common::transport::ZCM_UDPM_URL}, _m_joystick_uptr{std::move(joystick_uptr)}, _m_app_params{params}
{
  __init_signals();
  __validate_app_params();

  if (!_m_zcm_udpm.good())
  {
    throw std::runtime_error("ZCM initialization failed.");
  }
}

JoystickService::~JoystickService()
{
  // Only if we set to auto repeat joystick messages
  // do we want to kill the thread that does the same.
  if (_m_publish_thread.joinable())
  {
    _m_publish_thread_tk.kill();
    _m_publish_thread.join();
  }
}

void JoystickService::init()
{
  uint8_t num_axes = _m_joystick_uptr->get_num_axes();
  uint8_t num_buttons = _m_joystick_uptr->get_num_buttons();

  _m_joy_msg.nAxes = static_cast<int8_t>(num_axes);
  _m_joy_msg.nButtons = static_cast<int8_t>(num_buttons);
  _m_last_published_joy_msg.nAxes = static_cast<int8_t>(num_axes);
  _m_last_published_joy_msg.nButtons = static_cast<int8_t>(num_buttons);
  _m_sticky_buttons_joy_msg.nAxes = static_cast<int8_t>(num_axes);
  _m_sticky_buttons_joy_msg.nButtons = static_cast<int8_t>(num_buttons);

  _m_joy_msg.axes.resize(num_axes, 0.0f);
  _m_joy_msg.buttons.resize(num_buttons, 0);
  _m_last_published_joy_msg.axes.resize(num_axes, 0.0f);
  _m_last_published_joy_msg.buttons.resize(num_buttons, 0);
  _m_sticky_buttons_joy_msg.axes.resize(num_axes, 0.0f);
  _m_sticky_buttons_joy_msg.buttons.resize(num_buttons, 0);

  std::cout << "Connected to " << _m_joystick_uptr->get_name() << ' ';
  std::cout << "(Version = " << _m_joystick_uptr->get_version() << ')';
  std::cout << std::endl;

  if (_m_app_params.auto_repeat_rate > 0)
  {
    _m_publish_thread = std::thread{&JoystickService::__publish_joystick_data_thread, this};
  }
}

void JoystickService::run()
{
  auto max_value = static_cast<double>(std::numeric_limits<int16_t>::max());
  double scale = -1.0 / (1.0 - _m_app_params.deadzone) / max_value;
  double unscaled_deadzone = max_value * _m_app_params.deadzone;

  bool tv_set = false;
  bool publication_pending = false;

  _m_joystick_uptr->set_timeval(1, 0);

  while (!_m_stop_loop.load())
  {
    bool publish_now = false;
    bool publish_soon = false;

    if (!_m_joystick_uptr->is_event_ready())
    {
      _m_joystick_uptr->set_timeval(0, 0);
      continue;
      // Joystick is probably closed. Not sure if this case is useful.
      // break;
    }

    JoystickEvent event{};
    if (!_m_joystick_uptr->sample(event) && errno == EINTR)
    {
      // Joystick is probably closed (definitely occurs) or there was interrupt.
      break;
    }

    if (_m_joystick_uptr->is_fd_set())
    {
      {
        std::lock_guard<std::mutex> guard(_m_joy_msg_mutex);
        _m_joy_msg.utime = common::time::now();
      }

      if (event.is_button() || event.is_button_init())
      {
        button_event_t button_event = event.get_button_event();

        {
          std::lock_guard<std::mutex> guard(_m_joy_msg_mutex);
          _m_joy_msg.buttons[button_event.first] = static_cast<int8_t>(button_event.second);
        }

        // For initial events, wait a bit before sending to try to catch all the initial events.
        publish_now = !event.is_init();
        publish_soon = event.is_init();
      }
      else if (event.is_axis() || event.is_axis_init())
      {
        axis_event_t axis_event = event.get_axis_event();

        double axis_value = axis_event.second;
        // Allows deadzone to be "smooth"
        if (axis_value > unscaled_deadzone)
        {
          axis_value -= unscaled_deadzone;
        }
        else if (axis_value < -unscaled_deadzone)
        {
          axis_value += unscaled_deadzone;
        }
        else
        {
          axis_value = 0.0;
        }

        publish_soon = true;

        {
          std::lock_guard<std::mutex> guard(_m_joy_msg_mutex);
          _m_joy_msg.axes[axis_event.first] = static_cast<float>(axis_value * scale);
        }
      }
    }
    else if (tv_set)
    {
      publish_now = true;
    }

    if (publish_now)
    {
      // Assume that all the init events have arrived already. This should be
      // the case as the kernel sends them along as soon as the device opens.
      if (_m_app_params.sticky_buttons)
      {
        std::lock_guard<std::mutex> guard(_m_joy_msg_mutex);
        // Cycle through buttons
        for (size_t i = 0; i < _m_joy_msg.buttons.size(); ++i)
        {
          // Change button state only on transition from 0 to 1
          if (_m_joy_msg.buttons[i] == 1 && _m_last_published_joy_msg.buttons[i] == 0)
          {
            _m_sticky_buttons_joy_msg.buttons[i] = static_cast<int8_t>(_m_sticky_buttons_joy_msg.buttons[i] != 0);
          }
          else
          {
            // Do not change the message sate
          }
        }
        // Update last published message
        _m_last_published_joy_msg = _m_joy_msg;
        // Fill rest of sticky_buttons_joy_msg (time stamps, axes, etc)
        _m_sticky_buttons_joy_msg.utime = _m_joy_msg.utime;
        _m_sticky_buttons_joy_msg.axes.assign(_m_joy_msg.axes.begin(), _m_joy_msg.axes.end());

        if (_m_app_params.auto_repeat_rate == 0)
        {
          _m_zcm_udpm.publish("CRAEBOT_JOYSTICK", &_m_sticky_buttons_joy_msg);
        }
      }
      else if (_m_app_params.auto_repeat_rate == 0)
      {
        _m_zcm_udpm.publish("CRAEBOT_JOYSTICK", &_m_joy_msg);
      }

      tv_set = false;
      publication_pending = false;
      publish_soon = false;
    }

    // If an axis event occurred, start a timer to combine with other events (only when auto-repeating is disabled).
    if (!publication_pending && publish_soon && _m_app_params.auto_repeat_rate == 0)
    {
      auto seconds = static_cast<time_t>(trunc(_m_app_params.coalesce_interval));
      auto useconds = static_cast<suseconds_t>((_m_app_params.coalesce_interval - seconds) * 1e6);
      _m_joystick_uptr->set_timeval(seconds, useconds);
      publication_pending = true;
      tv_set = true;
    }

    if (!tv_set)
    {
      _m_joystick_uptr->set_timeval(1, 0);
    }
  }

  std::cout << "The joystick node will be shutting down now." << std::endl;
}

void JoystickService::__publish_joystick_data_thread()
{
  auto auto_repeat_sleep_time = static_cast<int64_t>(1.0 / _m_app_params.auto_repeat_rate * 1000.0);
  auto auto_repeat_sleep_ms = std::chrono::milliseconds{auto_repeat_sleep_time};

  while (_m_publish_thread_tk.wait_for(auto_repeat_sleep_ms))
  {
    std::lock_guard<std::mutex> guard(_m_joy_msg_mutex);
    if (_m_app_params.sticky_buttons)
    {
      _m_zcm_udpm.publish("CRAEBOT_JOYSTICK", &_m_sticky_buttons_joy_msg);
    }
    else
    {
      _m_zcm_udpm.publish("CRAEBOT_JOYSTICK", &_m_joy_msg);
    }
  }
}

void JoystickService::__init_signals() const
{
  std::signal(SIGTERM, JoystickService::_s_signal_handler);
  std::signal(SIGKILL, JoystickService::_s_signal_handler);
  std::signal(SIGINT, JoystickService::_s_signal_handler);
}

void JoystickService::__validate_app_params()
{
  if (_m_app_params.deadzone >= 1.0)
  {
    std::cerr << "Normalizing the deadzone value." << std::endl;
    _m_app_params.deadzone /= std::numeric_limits<int16_t>::max();
  }

  if (_m_app_params.deadzone > 0.9)
  {
    std::cerr << "Deadzone cannot be greater than 0.9." << std::endl;
    _m_app_params.deadzone = 0.9;
  }

  if (_m_app_params.deadzone < 0.0)
  {
    std::cerr << "The deadzone value cannot be less than zero." << std::endl;
    _m_app_params.deadzone = 0;
  }

  if (_m_app_params.auto_repeat_rate < 0.0)
  {
    std::cerr << "The auto repeat rate cannot be less than zero." << std::endl;
    _m_app_params.auto_repeat_rate = 0.0;
  }

  if (_m_app_params.coalesce_interval < 0.0)
  {
    std::cerr << "The coalesce rate cannot be less than zero." << std::endl;
    _m_app_params.coalesce_interval = 0.0;
  }
}

void JoystickService::_s_signal_handler(int /*signal*/)
{
  std::cout << "Goodbye!" << std::endl;
  _m_stop_loop.store(true);
}

}  // namespace joystick
}  // namespace sensors
}  // namespace craebot
