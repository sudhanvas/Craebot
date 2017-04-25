#include "joystick.hpp"

namespace craebot {
namespace sensors {

sig_atomic_t Joystick::_m_quit = false;

Joystick::Joystick(app_params_t& params)
    : _m_deadzone(params.deadzone)
    ,  _m_auto_repeat_rate(params.auto_repeat_rate)
    ,  _m_coalesce_interval(params.coalesce_interval)
    ,  _m_device_filename(params.device_filename) {
  _init_signals();

  if (_m_deadzone >= 1.0) {
    fprintf(stderr, "Normalizing the deadzone value.\n");
    _m_deadzone /= std::numeric_limits<short>::max();
  }
  if (_m_deadzone > 0.9) {
    fprintf(stderr, "Deadzone cannot be greater than 0.9.\n");
    _m_deadzone = 0.9;
  }
  if (_m_deadzone < 0.0) {
    fprintf(stderr, "The deadzone value cannot be less than zero.\n");
    _m_deadzone = 0;
  }
  if (_m_auto_repeat_rate > 1 / _m_coalesce_interval) {
    fprintf(stderr, "Timing behavior is not well defined.\n");
  }
  if (_m_auto_repeat_rate < 0.0) {
    fprintf(stderr, "The auto repeat rate cannot be less than zero.\n");
    _m_auto_repeat_rate = 0.0;
  }
  if (_m_coalesce_interval < 0.0) {
    fprintf(stderr, "The coalesce rate cannot be less than zero.\n");
    _m_coalesce_interval = 0.0;
  }

  if (!_m_lcm.good()) {
    fprintf(stderr, "LCM initialization failed.\n");
    exit (EXIT_FAILURE);
  }
}

void Joystick::run() {
  float max_value = static_cast<float> (std::numeric_limits<short>::max());
  float auto_repeat_interval = 1 / _m_auto_repeat_rate;
  float scale = -1.0f / (1.0f - _m_deadzone) / max_value;
  float unscaled_deadzone = max_value * _m_deadzone;

  js_event event;
  struct timeval tv;
  fd_set set;
  int joy_fd;

  while (!_m_quit) {
    bool first_fault = true;

    while (true) {
      if (_m_quit) {
        printf("The joystick node will be shutting down now.\n");
        exit (EXIT_SUCCESS);
      }

      joy_fd = open(_m_device_filename.c_str(), O_RDONLY);
      if (joy_fd != -1) {
        close(joy_fd);
        joy_fd = open(_m_device_filename.c_str(), O_RDONLY);
      }
      if (joy_fd != -1) {
        break;
      }
      if (first_fault) {
        fprintf(stderr,
                "Couldn't open the joystick. Will retry every second.\n");
        first_fault = false;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    printf("Opened joystick with filename %s\n", _m_device_filename.c_str());
    printf("The deadzone for this joystick is %.2f.\n", _m_deadzone);
    printf("The auto repeat rate (in Hz)  %.2f.\n", _m_auto_repeat_rate);
    printf("The coalesce interval (in seconds) is %.2f.\n", _m_coalesce_interval);

    bool tv_set = false;
    bool publication_pending = false;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    joystick_t joystick_message;
    joystick_message.nAxes = 0;
    joystick_message.nButtons = 0;

    while (!_m_quit) {
      bool publish_now = false;
      bool publish_soon = false;
      FD_ZERO(&set);
      FD_SET(joy_fd, &set);

      int select_out = select(joy_fd+1, &set, NULL, NULL, &tv);
      if (select_out == -1) {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        printf("Select returned negative.\n");
        continue;
      }

      if (FD_ISSET(joy_fd, &set)) {
        if (read(joy_fd, &event, sizeof(js_event)) == -1 && errno != EAGAIN) {
          break;
        }

        joystick_message.utime = craebot::common::now();

        switch(event.type) {
          case JS_EVENT_BUTTON:
          case JS_EVENT_BUTTON | JS_EVENT_INIT:
            if(event.number >= joystick_message.nButtons) {
              int8_t old_size = joystick_message.nButtons;
              joystick_message.nButtons =
                  static_cast<int8_t> (event.number + 1);
              joystick_message.buttons.resize(
                  (size_t) joystick_message.nButtons);

              for(int8_t i = old_size; i < joystick_message.nButtons; ++i) {
                joystick_message.buttons[i] = false;
              }
            }

            joystick_message.buttons[event.number] = event.value != 0;
            if (!(event.type & JS_EVENT_INIT)) {
              publish_now = true;
            }
            else {
              publish_soon = true;
            }
            break;
          case JS_EVENT_AXIS:
          case JS_EVENT_AXIS | JS_EVENT_INIT:
            if(event.number >= joystick_message.nAxes) {
              int8_t old_size = joystick_message.nAxes;
              joystick_message.nAxes = static_cast<int8_t> (event.number + 1);
              joystick_message.axes.resize((size_t) joystick_message.nAxes);

              for(int8_t i = old_size; i < joystick_message.nAxes; ++i) {
                joystick_message.axes[i] = 0.0;
              }
            }
            if (!(event.type & JS_EVENT_INIT)) {
              float axis_value = event.value;

              if (axis_value > unscaled_deadzone) {
                axis_value -= unscaled_deadzone;
              }
              else if (axis_value < -unscaled_deadzone) {
                axis_value += unscaled_deadzone;
              }
              else {
                axis_value = 0.0;
              }

              joystick_message.axes[event.number] = axis_value * scale;
            }
            publish_soon = true;
            break;
          default:
            printf("Unknown event type recorded.\n"
                "    Event Time: %d\n"
                "    Event Type: %d\n"
                "    Event Value: %d\n"
                "    Event Number: %d\n",
                event.time,
                event.type,
                event.value,
                event.number);
            break;
        }
      }
      else if (tv_set) {
        publish_now = true;
      }

      if (publish_now) {
        _m_lcm.publish("CRAEBOT_JOYSTICK", &joystick_message);
        tv_set = false;
        publication_pending = false;
        publish_soon = false;
      }

      if (!publication_pending && publish_soon) {
        tv.tv_sec = static_cast<__time_t> (trunc(_m_coalesce_interval));
        tv.tv_usec = static_cast<__suseconds_t> ((_m_coalesce_interval - tv.tv_sec) * 1e6);
        publication_pending = true;
        tv_set = true;
      }

      if (!tv_set && _m_auto_repeat_rate > 0.0) {
        tv.tv_sec = static_cast<__time_t> (trunc(auto_repeat_interval));
        tv.tv_usec = static_cast<__suseconds_t> ((auto_repeat_interval - tv.tv_sec) * 1e6);
        tv_set = true;
      }

      if (!tv_set) {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
      }
    }

    close(joy_fd);
    if (!_m_quit) {
      fprintf(stderr,
              "Connection to the joystick device was lost unexpectedly.\n");
      fprintf(stderr, "The program will try to reopen now.\n");
    }
  }
}

void Joystick::_init_signals(void) {
  _m_signal_action.sa_handler = _signal_handler;
  sigfillset(&_m_signal_action.sa_mask);
  _m_signal_action.sa_flags |= SA_SIGINFO;
  sigaction(SIGINT, &_m_signal_action, NULL);
}

void Joystick::_signal_handler(int signal) {
  if (signal == SIGINT) {
    printf("Goodbye!\n");
    _m_quit = true;
    std::exit (EXIT_SUCCESS);
  }
}

} // namespace sensors
} // namespace craebot


