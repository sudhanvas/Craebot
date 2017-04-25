#include <cinttypes>
#include <cstdio>

#include <lcm/lcm-cpp.hpp>

#include <lcmtypes/craebot/bumper_t.hpp>

inline const char * const boolalpha(bool boolean)
{
  return boolean ? "true" : "false";
}

class Handler {
 public:
  Handler() { }
  ~Handler() { }

  void callback(
      const lcm::ReceiveBuffer* rbuf,
      const std::string& channel,
      const craebot::bumper_t* bumper) {
    printf("utime: %" PRId64 "\n", bumper->utime);
    printf("is_left_pressed: %s\n", boolalpha(bumper->is_left_pressed));
    printf("is_right_pressed: %s\n", boolalpha(bumper->is_right_pressed));

    printf("is_light_left: %s\n", boolalpha(bumper->is_light_left));
    printf("is_light_front_left: %s\n", boolalpha(bumper->is_light_front_left));
    printf("is_light_center_left: %s\n", boolalpha(bumper->is_light_center_left));
    printf("is_light_center_right: %s\n", boolalpha(bumper->is_light_center_right));
    printf("is_light_front_right: %s\n", boolalpha(bumper->is_light_front_right));
    printf("is_light_right: %s\n", boolalpha(bumper->is_light_right));

    printf("light_signal_left: %d\n", bumper->light_signal_left);
    printf("light_signal_front_left: %d\n", bumper->light_signal_front_left);
    printf("light_signal_center_left: %d\n", bumper->light_signal_center_left);
    printf("light_signal_center_right: %d\n", bumper->light_signal_center_right);
    printf("light_signal_front_right: %d\n", bumper->light_signal_front_right);
    printf("light_signal_right: %d\n", bumper->light_signal_right);

    printf("omni_ir: %d\n\n", bumper->omni_ir);
  }
};


int main(void) {
  lcm::LCM lcm;
  if (!lcm.good()) {
    fprintf(stderr, "Failed to initialize LCM.\n");
    return EXIT_FAILURE;
  }

  Handler handler;
  lcm.subscribe("CRAEBOT_BUMPER", &Handler::callback, &handler);
  printf("Subscribed to channel: CRAEBOT_BUMPER\n");

  while (!lcm.handle());

  return EXIT_SUCCESS;
}
