#include "joystick.hh"

namespace craebot
{
namespace sensors
{
namespace joystick
{

Joystick::Joystick() : _m_device_fd{-1}, _m_fd_set{}, _m_fd_tv{}
{
}

Joystick::~Joystick()
{
  close();
}

void Joystick::open(const std::string &device_filename, const bool blocking)
{
  __open_device(device_filename, blocking);
}

bool Joystick::connected() const noexcept
{
  return _m_device_fd > 0;
}

void Joystick::close()
{
  ::close(_m_device_fd);
}

std::string Joystick::get_name() const
{
  char name[256]{};
  ::ioctl(_m_device_fd, JSIOCGNAME(256), &name[0]);  // NOLINT
  return std::string(&name[0]);
}

uint32_t Joystick::get_version() const
{
  uint32_t version{};
  ::ioctl(_m_device_fd, JSIOCGVERSION, &version);  // NOLINT
  return version;
}

uint8_t Joystick::get_num_axes() const
{
  uint8_t num_axes{};
  ::ioctl(_m_device_fd, JSIOCGAXES, &num_axes);  // NOLINT
  return num_axes;
}

uint8_t Joystick::get_num_buttons() const
{
  uint8_t num_buttons{};
  ::ioctl(_m_device_fd, JSIOCGBUTTONS, &num_buttons);  // NOLINT
  return num_buttons;
}

bool Joystick::is_event_ready()
{
  FD_ZERO(&_m_fd_set);               // NOLINT
  FD_SET(_m_device_fd, &_m_fd_set);  // NOLINT

  return select(_m_device_fd + 1, &_m_fd_set, nullptr, nullptr, &_m_fd_tv) != -1;
}

bool Joystick::is_fd_set() const
{
  return static_cast<bool>(FD_ISSET(_m_device_fd, &_m_fd_set));  // NOLINT
}

bool Joystick::sample(JoystickEvent &event)
{
  return !(read(_m_device_fd, &event, sizeof(event)) == -1 && errno != EAGAIN);
}

void Joystick::set_timeval(const time_t &seconds, const suseconds_t &useconds)
{
  _m_fd_tv.tv_sec = seconds;
  _m_fd_tv.tv_usec = useconds;
}

void Joystick::__open_device(const std::string &device_filename, bool blocking)
{
  uint8_t count = 0;
  bool first_fault = true;

  while (!connected() && count < 10)
  {
    errno = EXIT_SUCCESS;
    _m_device_fd = ::open(device_filename.c_str(), blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK);  // NOLINT

    if (errno)
    {
      std::cerr << std::strerror(errno) << std::endl;

      if (first_fault)
      {
        std::cerr << "Couldn't open the joystick. Will retry every second (for 10 seconds)." << std::endl;
        first_fault = false;
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ++count;
  }
}

}  // namespace joystick
}  // namespace sensors
}  // namespace craebot
