#include <cinttypes>
#include <cstdio>

#include <lcm/lcm-cpp.hpp>

#include <lcmtypes/craebot/battery_state_t.hpp>

class Handler {
 public:
  Handler() { }
  ~Handler() { }

  void callback(
      const lcm::ReceiveBuffer* rbuf,
      const std::string& channel,
      const craebot::battery_state_t* battery) {
    printf("utime: %" PRId64 "\n", battery->utime);
    printf("voltage:        %.4f\n", battery->voltage);
    printf("current:        %.4f\n", battery->current);
    printf("charge:         %.4f\n", battery->charge);
    printf("capacity:       %.4f\n", battery->capacity);
    printf("ratio:          %.4f\n", battery->charge_ratio);
    printf("temperature:    %d  \n", battery->temperature);
    printf("charging_state: %d  \n", battery->charging_state);
    printf("\n");
  }
};

int main(void) {
  lcm::LCM lcm;
  if (!lcm.good()) {
    fprintf(stderr, "Failed to initialize LCM.\n");
    return EXIT_FAILURE;
  }

  Handler handler;
  lcm.subscribe("CRAEBOT_BATTERY_STATE", &Handler::callback, &handler);
  printf("Subscribed to channel: CRAEBOT_BATTERY_STATE\n");

  while (!lcm.handle());

  return EXIT_SUCCESS;
}
