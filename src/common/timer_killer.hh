#ifndef CRAEBOT_COMMON_TIMER_KILLER_HH_
#define CRAEBOT_COMMON_TIMER_KILLER_HH_

#include <chrono>
#include <condition_variable>
#include <mutex>

#include <boost/noncopyable.hpp>

namespace craebot
{
namespace common
{
namespace threading
{

class TimerKiller : private boost::noncopyable
{
 public:
  TimerKiller() : _m_terminate{false}
  {
  }

  inline bool wait_for(std::chrono::milliseconds const &time)
  {
    std::unique_lock<std::mutex> lock{_m_mutex};
    return !_m_cv.wait_for(lock, time, [&] { return _m_terminate; });
  }

  inline void kill()
  {
    std::unique_lock<std::mutex> lock(_m_mutex);
    _m_terminate = true;
    _m_cv.notify_all();
  }

 private:
  std::condition_variable _m_cv;
  std::mutex _m_mutex;
  bool _m_terminate;
};

}  // namespace threading
}  // namespace common
}  // namespace craebot

#endif  // CRAEBOT_COMMON_TIMER_KILLER_HH_
