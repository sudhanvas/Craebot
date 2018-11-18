#ifndef CRAEBOT_SENSORS_JOYSTICK_FACTORY_HH_
#define CRAEBOT_SENSORS_JOYSTICK_FACTORY_HH_

#include <memory>

#include "joystick.hh"

namespace craebot
{
namespace sensors
{
namespace joystick
{
namespace factory
{

std::unique_ptr<craebot::sensors::joystick::Joystick> create_concrete_joystick(const std::string &device_filename)
{
  auto joystick_uptr = std::make_unique<craebot::sensors::joystick::Joystick>();

  joystick_uptr->open(device_filename);
  if (!joystick_uptr->connected())
  {
    throw std::runtime_error("Failed to open the joystick.");
  }

  return joystick_uptr;
}

}  // namespace factory
}  // namespace joystick
}  // namespace sensors
}  // namespace craebot

#endif  // CRAEBOT_SENSORS_JOYSTICK_FACTORY_HH_
