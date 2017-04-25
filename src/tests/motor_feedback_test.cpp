#include <cstdio>
#include <cinttypes>

#include <lcm/lcm-cpp.hpp>

#include <lcmtypes/craebot/motor_feedback_t.hpp>

class Handler {
 public:
  Handler() { }
  ~Handler() { }

  void callback(
      const lcm::ReceiveBuffer* rbuf,
      const std::string& channel,
      const craebot::motor_feedback_t* feedback) {
    printf("  utime: %" PRId64 "\n", feedback->utime);
    printf("  [left, right]_wheel_position:\t\t%f,\t%f\n",
           feedback->left_wheel_position,
           feedback->right_wheel_position);
    printf("  [left, right]_wheel_speed:\t%f,\t%f\n",
           feedback->left_wheel_speed,
           feedback->right_wheel_speed);
  }
};

int main(void) {
  lcm::LCM lcm;
  if (!lcm.good()) {
    fprintf(stderr, "Failed to initialize LCM.\n");
    return EXIT_FAILURE;
  }

  Handler handler;
  lcm.subscribe("CRAEBOT_MOTOR_FEEDBACK", &Handler::callback, &handler);
  printf("Subscribed to channel: CRAEBOT_MOTOR_FEEDBACK\n");

  while (!lcm.handle());

  return EXIT_SUCCESS;
}
