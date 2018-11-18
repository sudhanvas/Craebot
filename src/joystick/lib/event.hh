#ifndef CRAEBOT_SENSORS_JOYSTICK_EVENT_HH_
#define CRAEBOT_SENSORS_JOYSTICK_EVENT_HH_

#include <unistd.h>
#include <linux/joystick.h>

#include <cstdint>
#include <iostream>
#include <utility>

#include <boost/noncopyable.hpp>

namespace craebot
{
namespace sensors
{
namespace joystick
{
using button_event_t = std::pair<uint8_t, bool>;
using axis_event_t = std::pair<uint8_t, int16_t>;

class JoystickEvent : public ::js_event, private boost::noncopyable
{
 public:
  JoystickEvent() = default;
  ~JoystickEvent() = default;

  bool is_init() const;
  bool is_button() const;
  bool is_axis() const;
  bool is_button_init() const;
  bool is_axis_init() const;

  button_event_t get_button_event() const;
  axis_event_t get_axis_event() const;

  friend std::ostream &operator<<(std::ostream &os, const JoystickEvent &event);
};

}  // namespace joystick
}  // namespace sensors
}  // namespace craebot

#endif  // CRAEBOT_SENSORS_JOYSTICK_EVENT_HH_
