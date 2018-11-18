#include <iomanip>
#include <iostream>

#include <boost/noncopyable.hpp>
#include <zcm/zcm-cpp.hpp>

#include "common/transport.hh"
#include "zcmtypes/craebot/odometry_t.hpp"

class Handler : private boost::noncopyable
{
 public:
  Handler() = default;
  ~Handler() = default;

  void callback(const zcm::ReceiveBuffer * /*rbuf*/,
                const std::string & /*channel*/,
                const craebot::odometry_t *odometry)
  {
    std::cout << "utime: " << odometry->utime << std::endl;

    std::cout << std::fixed << std::setprecision(4) << "x     = " << odometry->pose[0] << std::endl;
    std::cout << std::fixed << std::setprecision(4) << "y     = " << odometry->pose[1] << std::endl;
    std::cout << std::fixed << std::setprecision(4) << "theta = " << odometry->pose[2] << std::endl;

    std::cout << std::fixed << std::setprecision(4) << "dX     = " << odometry->velocity[0] << std::endl;
    std::cout << std::fixed << std::setprecision(4) << "dY     = " << odometry->velocity[1] << std::endl;
    std::cout << std::fixed << std::setprecision(4) << "dTheta = " << odometry->velocity[2] << std::endl;

    std::cout << std::fixed << std::setprecision(4) << "Pose Covariance: " << std::endl
              << "|" << odometry->pose_covariance[0] << odometry->pose_covariance[1] << odometry->pose_covariance[2]
              << '|' << std::endl
              << '|' << odometry->pose_covariance[3] << odometry->pose_covariance[4] << odometry->pose_covariance[5]
              << '|' << std::endl
              << '|' << odometry->pose_covariance[6] << odometry->pose_covariance[7] << odometry->pose_covariance[8]
              << '|' << std::endl;

    std::cout << std::fixed << std::setprecision(4) << "Velocity Covariance: " << std::endl
              << "|" << odometry->velocity_covariance[0] << odometry->velocity_covariance[1]
              << odometry->velocity_covariance[2] << '|' << std::endl
              << '|' << odometry->velocity_covariance[3] << odometry->velocity_covariance[4]
              << odometry->velocity_covariance[5] << '|' << std::endl
              << '|' << odometry->velocity_covariance[6] << odometry->velocity_covariance[7]
              << odometry->velocity_covariance[8] << '|' << std::endl;
  }
};

int main()
{
  zcm::ZCM zcm{craebot::common::transport::ZCM_UDPM_URL};

  if (!zcm.good())
  {
    std::cerr << "Failed to initialize ZCM\n";
    return EXIT_FAILURE;
  }

  Handler handler;
  zcm.subscribe("CRAEBOT_ODOMETRY", &Handler::callback, &handler);
  std::cout << "Subscribed to channel: CRAEBOT_ODOMETRY" << std::endl;
  zcm.run();

  return EXIT_SUCCESS;
}
