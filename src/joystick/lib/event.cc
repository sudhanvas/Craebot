#include "event.hh"

namespace craebot
{
namespace sensors
{
namespace joystick
{
bool JoystickEvent::is_init() const
{
  return (type & JS_EVENT_INIT) != 0;
}

bool JoystickEvent::is_button() const
{
  return (type & JS_EVENT_BUTTON) != 0;
}

bool JoystickEvent::is_axis() const
{
  return (type & JS_EVENT_AXIS) != 0;
}

bool JoystickEvent::is_button_init() const
{
  return (type & (JS_EVENT_BUTTON | JS_EVENT_INIT)) != 0;
}

bool JoystickEvent::is_axis_init() const
{
  return (type & (JS_EVENT_AXIS | JS_EVENT_INIT)) != 0;
}

button_event_t JoystickEvent::get_button_event() const
{
  button_event_t event{};
  event.first = static_cast<uint8_t>(number);
  event.second = static_cast<bool>(value);
  return event;
}

axis_event_t JoystickEvent::get_axis_event() const
{
  axis_event_t event{};
  event.first = static_cast<uint8_t>(number);
  event.second = static_cast<int16_t>(value);
  return event;
}

std::ostream &operator<<(std::ostream &os, const JoystickEvent &event)
{
  os << "type=" << static_cast<int>(event.type) << ',';
  os << "number=" << static_cast<int>(event.number) << ',';
  os << "value=" << static_cast<int>(event.value) << ',';
  return os;
}

}  // namespace joystick
}  // namespace sensors
}  // namespace craebot
