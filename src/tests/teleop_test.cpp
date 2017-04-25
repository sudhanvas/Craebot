#include <atomic>
#include <chrono>
#include <cstdio>
#include <mutex>
#include <thread>

#include <lcm/lcm-cpp.hpp>

#include <common/timestamp.hpp>
#include <lcmtypes/craebot/teleop_t.hpp>

static const float LINEAR_SPEED = 0.25f;
static const float ANGULAR_SPEED = 2.125f;
static const float PUBLISH_FREQUENCY = 5.0f;
static const float COMMAND_STOP = 0.0f;

using ms = std::chrono::milliseconds;
using sec = std::chrono::seconds;

class TeleopTest {
 private:
  lcm::LCM _m_lcm;

  craebot::teleop_t _m_teleop_msg;
  std::thread* _m_publish_thread;
  std::mutex _m_data_mutex;
  std::atomic<bool> _m_quit;
  ms _m_publish_duration;

 public:
  TeleopTest(): _m_publish_thread(nullptr) {
    if (!_m_lcm.good()) {
      fprintf(stderr, "Failed to initialize LCM.\n");
      exit (EXIT_FAILURE);
    }

    _m_quit = false;
    _m_teleop_msg.utime = 0;
    _m_teleop_msg.linear_velocity = 0.0f;
    _m_teleop_msg.angular_velocity = 0.0f;

    long publish_duration_ms =
        static_cast<long> (1.0/PUBLISH_FREQUENCY * 1000.0);
    _m_publish_duration = ms(publish_duration_ms);
  }

  ~TeleopTest() {
    _m_publish_thread->join();
    delete _m_publish_thread;
    _m_publish_thread = nullptr;
  }

  void start() {
    _m_publish_thread = new std::thread(&TeleopTest::publish, this);

    // Move forward, @ 0.25 m/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = LINEAR_SPEED;
      _m_teleop_msg.angular_velocity = COMMAND_STOP;
    }
    std::this_thread::sleep_for(sec(5));

    // Move backward, @ 0.25 m/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = -LINEAR_SPEED;
      _m_teleop_msg.angular_velocity = COMMAND_STOP;
    }
    std::this_thread::sleep_for(sec(5));

    // Rotate left, @ 2.125 rad/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = COMMAND_STOP;
      _m_teleop_msg.angular_velocity = ANGULAR_SPEED;
    }
    std::this_thread::sleep_for(sec(5));

    // Rotate right, @ 2.125 rad/s, for 5 seconds.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = COMMAND_STOP;
      _m_teleop_msg.angular_velocity = -ANGULAR_SPEED;
    }
    std::this_thread::sleep_for(sec(5));

    // Stop the robot and wait 2 seconds for the command to propagate.
    {
      std::lock_guard<std::mutex> guard(_m_data_mutex);
      _m_teleop_msg.linear_velocity = COMMAND_STOP;
      _m_teleop_msg.angular_velocity = COMMAND_STOP;
    }
    std::this_thread::sleep_for(sec(1));

    _m_quit = true;
  }

  void publish() {
    while (!_m_quit) {
      {
        std::lock_guard<std::mutex> guard(_m_data_mutex);
        _m_teleop_msg.utime = craebot::common::now();
      }
      _m_lcm.publish("CRAEBOT_TELEOP", &_m_teleop_msg);
      std::this_thread::sleep_for(_m_publish_duration);
    }
  }
};

int main(void) {
  TeleopTest test;
  test.start();
  return 0;
}
