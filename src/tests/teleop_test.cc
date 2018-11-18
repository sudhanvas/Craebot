#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <thread>

#include <boost/noncopyable.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/timer_killer.hh"
#include "common/timestamp.hh"
#include "common/transport.hh"
#include "zcmtypes/craebot/teleop_t.hpp"

using namespace std::chrono_literals;  // NOLINT

class TeleopTest : private boost::noncopyable
{
 private:
  zcm::ZCM _m_zcm{craebot::common::transport::ZCM_UDPM_URL};

  craebot::teleop_t _m_teleop_msg;
  std::thread _m_publish_thread;
  std::mutex _m_data_mutex;
  craebot::common::threading::TimerKiller _m_publish_thread_tk;
  std::chrono::milliseconds _m_publish_duration;

 public:
  TeleopTest() : _m_publish_duration{}
  {
    if (!_m_zcm.good())
    {
      throw std::runtime_error("ZCM initialization failed.");
    }

    auto publish_duration_ms = static_cast<int64_t>(1.0 / 5000.0);  // 5 Hz
    _m_publish_duration = std::chrono::milliseconds(publish_duration_ms);
  }

  ~TeleopTest()
  {
    _m_publish_thread.join();
  }

  void start()
  {
    constexpr auto pi = static_cast<float>(M_PI);
    constexpr float linear_speed = 0.20f;
    constexpr float angular_speed = pi / 10.0f;
    constexpr float command_stop = 0.0f;

    _m_publish_thread = std::thread{&TeleopTest::publish, this};

    // Move forward, @ 0.20 m/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = linear_speed;
      _m_teleop_msg.angular_velocity = command_stop;
    }
    std::this_thread::sleep_for(5s);

    // Rotate 90-deg clockwise, @ pi/10 rad/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = command_stop;
      _m_teleop_msg.angular_velocity = -angular_speed;
    }
    std::this_thread::sleep_for(5s);

    // Move forward, @ 0.20 m/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = linear_speed;
      _m_teleop_msg.angular_velocity = command_stop;
    }
    std::this_thread::sleep_for(5s);

    // Rotate 90-deg clockwise, @ pi/10 rad/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = command_stop;
      _m_teleop_msg.angular_velocity = -angular_speed;
    }
    std::this_thread::sleep_for(5s);

    // Move forward, @ 0.20 m/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = linear_speed;
      _m_teleop_msg.angular_velocity = command_stop;
    }
    std::this_thread::sleep_for(5s);

    // Rotate 90-deg clockwise, @ pi/10 rad/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = command_stop;
      _m_teleop_msg.angular_velocity = -angular_speed;
    }
    std::this_thread::sleep_for(5s);

    // Move forward, @ 0.20 m/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = linear_speed;
      _m_teleop_msg.angular_velocity = command_stop;
    }
    std::this_thread::sleep_for(5s);

    // Rotate 90-deg clockwise, @ pi/10 rad/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = command_stop;
      _m_teleop_msg.angular_velocity = -angular_speed;
    }
    std::this_thread::sleep_for(5s);

    // Stop the robot and wait 20 seconds for propagating the messages.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = command_stop;
      _m_teleop_msg.angular_velocity = command_stop;
    }
    std::this_thread::sleep_for(1s);

    _m_publish_thread_tk.kill();
  }

  void publish()
  {
    while (_m_publish_thread_tk.wait_for(_m_publish_duration))
    {
      {
        std::lock_guard<std::mutex> guard(_m_data_mutex);
        _m_teleop_msg.utime = craebot::common::time::now();
      }
      _m_zcm.publish("CRAEBOT_TELEOP", &_m_teleop_msg);
    }
  }
};

int main()
{
  try
  {
    TeleopTest test;
    test.start();
  }
  catch (const std::exception &x)
  {
    std::cerr << "Teleop Test: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
