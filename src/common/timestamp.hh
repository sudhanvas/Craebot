#ifndef CRAEBOT_COMMON_TIMESTAMP_HH_
#define CRAEBOT_COMMON_TIMESTAMP_HH_

#include <chrono>

namespace craebot
{
namespace common
{
namespace time
{

inline int64_t now()
{
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

}  // namespace time
}  // namespace common
}  // namespace craebot

#endif  // CRAEBOT_COMMON_TIMESTAMP_HH_
