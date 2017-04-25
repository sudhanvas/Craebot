#include <cinttypes>
#include <cstdio>

#include <lcm/lcm-cpp.hpp>

#include <lcmtypes/craebot/odometry_t.hpp>

class Handler {
 public:
  Handler() { }
  ~Handler() { }

  void callback(
      const lcm::ReceiveBuffer* rbuf,
      const std::string& channel,
      const craebot::odometry_t* odometry) {
    printf("utime: %" PRId64 "\n", odometry->utime);
    printf("[ x,  y,  theta]: [%.4f, %.4f, %.4f]\n",
           odometry->pose[0],
           odometry->pose[1],
           odometry->pose[1]);
    printf("[dX, dY, dTheta]: [%.4f, %.4f, %.4f]\n",
           odometry->velocity[0],
           odometry->velocity[1],
           odometry->velocity[2]);
    printf("Pose Covariance:\n"
           "[%.4f, %.4f, %.4f\n"
           " %.4f, %.4f, %.4f\n"
           " %.4f, %.4f, %.4f]\n",
           odometry->pose_covariance[0],
           odometry->pose_covariance[1],
           odometry->pose_covariance[2],
           odometry->pose_covariance[3],
           odometry->pose_covariance[4],
           odometry->pose_covariance[5],
           odometry->pose_covariance[6],
           odometry->pose_covariance[7],
           odometry->pose_covariance[8]);
    printf("Velocity Covariance:\n"
           "[%.4f, %.4f, %.4f\n"
           " %.4f, %.4f, %.4f\n"
           " %.4f, %.4f, %.4f]\n",
           odometry->velocity_covariance[0],
           odometry->velocity_covariance[1],
           odometry->velocity_covariance[2],
           odometry->velocity_covariance[3],
           odometry->velocity_covariance[4],
           odometry->velocity_covariance[5],
           odometry->velocity_covariance[6],
           odometry->velocity_covariance[7],
           odometry->velocity_covariance[8]);
  }
};


int main(void) {
  lcm::LCM lcm;
  if (!lcm.good()) {
    fprintf(stderr, "Failed to initialize LCM.\n");
    return EXIT_FAILURE;
  }

  Handler handler;
  lcm.subscribe("CRAEBOT_ODOMETRY", &Handler::callback, &handler);
  printf("Subscribed to channel: CRAEBOT_ODOMETRY\n");

  while (!lcm.handle());

  return EXIT_SUCCESS;
}
