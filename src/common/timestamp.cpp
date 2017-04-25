#include "timestamp.hpp"

namespace craebot {
namespace common {

int64_t now(void) {
  std::chrono::time_point<std::chrono::system_clock> now =
      std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>
      (duration).count();
}

}
}
