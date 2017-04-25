#include <cinttypes>
#include <cstdio>

#include <lcm/lcm-cpp.hpp>

#include <lcmtypes/craebot/buttons_t.hpp>

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
      const craebot::buttons_t* buttons) {
    printf("utime:     %" PRId64 "\n", buttons->utime);
    printf("clean:     %s\n", boolalpha(buttons->clean));
    printf("cliff:     %s\n", boolalpha(buttons->cliff));
    printf("dock:      %s\n", boolalpha(buttons->dock));
    printf("day:       %s\n", boolalpha(buttons->day));
    printf("hour:      %s\n", boolalpha(buttons->hour));
    printf("minute:    %s\n", boolalpha(buttons->minute));
    printf("spot:      %s\n", boolalpha(buttons->spot));
    printf("wheeldrop: %s\n", boolalpha(buttons->wheeldrop));
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
  lcm.subscribe("CRAEBOT_BUTTONS", &Handler::callback, &handler);
  printf("Subscribed to channel: CRAEBOT_BUTTONS\n");

  while (!lcm.handle());

  return EXIT_SUCCESS;
}
