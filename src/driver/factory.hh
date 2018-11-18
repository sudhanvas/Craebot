#ifndef CRAEBOT_DRIVER_ROBOT_FACTORY_HH_
#define CRAEBOT_DRIVER_ROBOT_FACTORY_HH_

#include <memory>
#include <utility>

#include <create/create.h>

namespace craebot
{
namespace driver
{
namespace factory
{

using robot_t = std::pair<create::RobotModel, std::unique_ptr<create::Create>>;

robot_t create_concrete_robot(const std::string &model_name, const std::string &device_filename);

}  // namespace factory
}  // namespace driver
}  // namespace craebot

#endif  // CRAEBOT_DRIVER_ROBOT_FACTORY_HH_