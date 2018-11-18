#ifndef CRAEBOT_SENSORS_JOYSTICK_HH_
#define CRAEBOT_SENSORS_JOYSTICK_HH_

#include <fcntl.h>
#include <stdint.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include <boost/noncopyable.hpp>

#include "event.hh"

namespace craebot
{
namespace sensors
{
namespace joystick
{

class Joystick : private boost::noncopyable
{
 public:
  Joystick();
  ~Joystick();

  void open(const std::string& device_filename, bool blocking = true);
  bool connected() const noexcept;
  void close();

  std::string get_name() const;
  uint32_t get_version() const;
  uint8_t get_num_axes() const;
  uint8_t get_num_buttons() const;

  bool is_event_ready();
  bool is_fd_set() const;
  bool sample(JoystickEvent& event);

  void set_timeval(const time_t& seconds, const suseconds_t& useconds);

 private:
  int _m_device_fd;
  fd_set _m_fd_set;
  timeval _m_fd_tv;

 private:
  void __open_device(const std::string& device_filename, bool blocking = true);
};

}  // namespace joystick
}  // namespace sensors
}  // namespace craebot

#endif  // CRAEBOT_SENSORS_JOYSTICK_HH_
