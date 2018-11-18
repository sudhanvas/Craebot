#include "factory.hh"

namespace craebot
{
namespace driver
{
namespace factory
{

robot_t create_concrete_robot(const std::string &model_name, const std::string &device_filename)
{
  create::RobotModel robot_model{create::RobotModel::CREATE_2};
  std::unique_ptr<create::Create> robot_uptr;

  if (model_name == "roomba400")
  {
    robot_model = create::RobotModel::ROOMBA_400;
  }
  else if (model_name == "create1")
  {
    robot_model = create::RobotModel::CREATE_1;
  }
  else if (model_name == "create2")
  {
    robot_model = create::RobotModel::CREATE_2;
  }
  else
  {
    throw std::invalid_argument("the specified robot model name is not recognized");
  }

  robot_uptr = std::make_unique<create::Create>(robot_model);
  if (!robot_uptr->connect(device_filename, robot_model.getBaud()))
  {
    throw std::runtime_error("failed to establish a serial connection with Create");
  }

  return robot_t{robot_model, std::move(robot_uptr)};
}

}  // namespace factory
}  // namespace driver
}  // namespace craebot
