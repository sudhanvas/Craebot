#include "driver.hh"

namespace craebot
{
namespace driver
{

CraebotDriver::CraebotDriver(create::RobotModel robot_model,  // NOLINT
                             std::unique_ptr<create::Create> robot_uptr,
                             const double &sensor_data_frequency,
                             const double &battery_state_frequency)
    : _m_zcm_udpm{common::transport::ZCM_UDPM_URL}, _m_robot_model{robot_model}, _m_robot_uptr{std::move(robot_uptr)}
{
  __init_msgs();
  __init_robot();
  __init_publish_frequencies(sensor_data_frequency, battery_state_frequency);
}

CraebotDriver::~CraebotDriver()
{
  if (_m_robot_uptr->connected())
  {
    _m_robot_uptr->disconnect();
  }

  _m_publish_threads_tk.kill();
  _m_publish_sensor_data_thread.join();
  _m_publish_battery_state_thread.join();
  _m_zcm_udpm.stop();
}

void CraebotDriver::run()
{
  if (!_m_zcm_udpm.good())
  {
    throw std::runtime_error("ZCM initialization failed.");
  }

  _m_publish_sensor_data_thread = std::thread{&CraebotDriver::__publish_sensor_data_thread, this};
  _m_publish_battery_state_thread = std::thread{&CraebotDriver::__publish_battery_state_thread, this};
  _m_zcm_udpm.subscribe("CRAEBOT_TELEOP", &CraebotDriver::__teleop_callback, this);
  _m_zcm_udpm.subscribe("CRAEBOT_LEDS", &CraebotDriver::__leds_callback, this);
  _m_zcm_udpm.subscribe("CRAEBOT_ASCII", &CraebotDriver::__ascii_callback, this);
  _m_zcm_udpm.start();
}

void CraebotDriver::__init_msgs()
{
  std::fill_n(&_m_odometry_msg.pose_covariance[0], 9, 1e-5);
  std::fill_n(&_m_odometry_msg.velocity_covariance[0], 9, 1e-5);
}

void CraebotDriver::__init_robot()
{
  _m_robot_uptr->setMode(create::MODE_FULL);

  double current_battery_level = _m_robot_uptr->getBatteryCharge();
  current_battery_level /= _m_robot_uptr->getBatteryCapacity();
  current_battery_level *= 100.0;
  std::cout << "Current battery percentage is " << std::fixed << std::setprecision(2) << current_battery_level
            << std::endl;
}

void CraebotDriver::__init_publish_frequencies(const double &sensor_data_frequency,
                                               const double &battery_state_frequency)
{
  auto sensor_data_sleep_time = static_cast<int64_t>(1.0 / sensor_data_frequency * 1000.0);
  auto battery_state_sleep_time = static_cast<int64_t>(1.0 / battery_state_frequency * 1000.0);

  _m_sensor_data_sleep_ms = std::chrono::milliseconds(sensor_data_sleep_time);
  _m_battery_state_sleep_ms = std::chrono::milliseconds(battery_state_sleep_time);
}

void CraebotDriver::__teleop_callback(const zcm::ReceiveBuffer * /*rbuf*/,
                                      const std::string & /*channel*/,
                                      const teleop_t *teleop)
{
  std::lock_guard<std::mutex> guard(_m_robot_mutex);
  _m_robot_uptr->drive(teleop->linear_velocity, teleop->angular_velocity);
}

void CraebotDriver::__leds_callback(const zcm::ReceiveBuffer * /*rbuf*/,
                                    const std::string & /*channel*/,
                                    const leds_t *leds)
{
  std::lock_guard<std::mutex> guard(_m_robot_mutex);
  _m_robot_uptr->enableDebrisLED(static_cast<bool>(leds->debris));
  _m_robot_uptr->enableSpotLED(static_cast<bool>(leds->spot));
  _m_robot_uptr->enableDockLED(static_cast<bool>(leds->dock));
  _m_robot_uptr->enableCheckRobotLED(static_cast<bool>(leds->check));
  _m_robot_uptr->setPowerLED(leds->power[0], leds->power[1]);
}

void CraebotDriver::__ascii_callback(const zcm::ReceiveBuffer * /*rbuf*/,
                                     const std::string & /*channel*/,
                                     const ascii_t *ascii)
{
  bool result = true;

  switch (ascii->nCharacters)
  {
    case 0:
      std::cerr << "No ASCII characters provided.\n";
      break;
    case 1:
      result = _m_robot_uptr->setDigitsASCII(ascii->data[0], ' ', ' ', ' ');
      break;
    case 2:
      result = _m_robot_uptr->setDigitsASCII(ascii->data[0], ascii->data[1], ' ', ' ');
      break;
    case 3:
      result = _m_robot_uptr->setDigitsASCII(ascii->data[0], ascii->data[1], ascii->data[2], ' ');
      break;
    case 4:
      result = _m_robot_uptr->setDigitsASCII(ascii->data[0], ascii->data[1], ascii->data[2], ascii->data[3]);
      break;
    default:
      result = false;
      break;
  }

  if (!result)
  {
    std::cerr << "ASCII character out of range [32, 126].\n";
  }
}

void CraebotDriver::__publish_sensor_data_thread()
{
  do
  {
    __publish_motor_feedback();
    __publish_bumper_data();
    __publish_odometry_data();
    __publish_button_states();
  } while (_m_publish_threads_tk.wait_for(_m_sensor_data_sleep_ms));
}

void CraebotDriver::__publish_battery_state_thread()
{
  do
  {
    {
      std::lock_guard<std::mutex> guard(_m_robot_mutex);
      _m_battery_state_msg.utime = common::time::now();
      _m_battery_state_msg.voltage = _m_robot_uptr->getVoltage();
      _m_battery_state_msg.current = _m_robot_uptr->getCurrent();
      _m_battery_state_msg.charge = _m_robot_uptr->getBatteryCharge();
      _m_battery_state_msg.capacity = _m_robot_uptr->getBatteryCapacity();
      _m_battery_state_msg.temperature = _m_robot_uptr->getTemperature();
      _m_battery_state_msg.charging_state = static_cast<uint8_t>(_m_robot_uptr->getChargingState());
    }
    _m_battery_state_msg.charge_ratio = _m_battery_state_msg.charge;
    _m_battery_state_msg.charge_ratio /= _m_battery_state_msg.capacity;
    _m_zcm_udpm.publish("CRAEBOT_BATTERY_STATE", &_m_battery_state_msg);
  } while (_m_publish_threads_tk.wait_for(_m_battery_state_sleep_ms));
  ;
}

void CraebotDriver::__publish_motor_feedback()
{
  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);
    _m_motor_feedback_msg.utime = common::time::now();
    _m_motor_feedback_msg.left_wheel_position = _m_robot_uptr->getLeftWheelDistance() / _m_wheel_radius;
    _m_motor_feedback_msg.right_wheel_position = _m_robot_uptr->getRightWheelDistance() / _m_wheel_radius;
    _m_motor_feedback_msg.left_wheel_speed = _m_robot_uptr->getRequestedLeftWheelVel() / _m_wheel_radius;
    _m_motor_feedback_msg.right_wheel_speed = _m_robot_uptr->getRequestedRightWheelVel() / _m_wheel_radius;
  }
  _m_zcm_udpm.publish("CRAEBOT_MOTOR_FEEDBACK", &_m_motor_feedback_msg);
}

void CraebotDriver::__publish_bumper_data()
{
  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);

    _m_bumper_msg.utime = common::time::now();
    _m_bumper_msg.is_left_pressed = static_cast<int8_t>(_m_robot_uptr->isLeftBumper());
    _m_bumper_msg.is_right_pressed = static_cast<int8_t>(_m_robot_uptr->isRightBumper());

    if (_m_robot_model.getVersion() >= create::V_3)
    {
      _m_bumper_msg.is_light_left = static_cast<int8_t>(_m_robot_uptr->isLightBumperLeft());
      _m_bumper_msg.is_light_front_left = static_cast<int8_t>(_m_robot_uptr->isLightBumperFrontLeft());
      _m_bumper_msg.is_light_center_left = static_cast<int8_t>(_m_robot_uptr->isLightBumperCenterLeft());
      _m_bumper_msg.is_light_right = static_cast<int8_t>(_m_robot_uptr->isLightBumperRight());
      _m_bumper_msg.is_light_front_right = static_cast<int8_t>(_m_robot_uptr->isLightBumperFrontRight());
      _m_bumper_msg.is_light_center_right = static_cast<int8_t>(_m_robot_uptr->isLightBumperCenterRight());

      _m_bumper_msg.light_signal_left = _m_robot_uptr->getLightSignalLeft();
      _m_bumper_msg.light_signal_front_left = _m_robot_uptr->getLightSignalFrontLeft();
      _m_bumper_msg.light_signal_center_left = _m_robot_uptr->getLightSignalCenterLeft();
      _m_bumper_msg.light_signal_right = _m_robot_uptr->getLightSignalRight();
      _m_bumper_msg.light_signal_front_right = _m_robot_uptr->getLightSignalFrontRight();
      _m_bumper_msg.light_signal_center_right = _m_robot_uptr->getLightSignalCenterRight();

      _m_bumper_msg.omni_ir = _m_robot_uptr->getIROmni();
    }
  }

  _m_zcm_udpm.publish("CRAEBOT_BUMPER", &_m_bumper_msg);
}

void CraebotDriver::__publish_odometry_data()
{
  create::Pose pose;
  create::Vel velocity;

  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);
    _m_odometry_msg.utime = common::time::now();
    pose = _m_robot_uptr->getPose();
    velocity = _m_robot_uptr->getVel();
  }

  _m_odometry_msg.pose[0] = pose.x;
  _m_odometry_msg.pose[1] = pose.y;
  _m_odometry_msg.pose[2] = pose.yaw;

  _m_odometry_msg.velocity[0] = velocity.x;
  _m_odometry_msg.velocity[1] = velocity.y;
  _m_odometry_msg.velocity[2] = velocity.yaw;

  std::copy(pose.covariance.begin(), pose.covariance.end(), &_m_odometry_msg.pose_covariance[0]);
  std::copy(velocity.covariance.begin(), velocity.covariance.end(), &_m_odometry_msg.velocity_covariance[0]);

  _m_zcm_udpm.publish("CRAEBOT_ODOMETRY", &_m_odometry_msg);
}

void CraebotDriver::__publish_button_states()
{
  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);
    _m_buttons_msg.utime = common::time::now();
    _m_buttons_msg.clean = static_cast<int8_t>(_m_robot_uptr->isCleanButtonPressed());
    _m_buttons_msg.day = static_cast<int8_t>(_m_robot_uptr->isDayButtonPressed());
    _m_buttons_msg.hour = static_cast<int8_t>(_m_robot_uptr->isHourButtonPressed());
    _m_buttons_msg.minute = static_cast<int8_t>(_m_robot_uptr->isMinButtonPressed());
    _m_buttons_msg.dock = static_cast<int8_t>(_m_robot_uptr->isDockButtonPressed());
    _m_buttons_msg.spot = static_cast<int8_t>(_m_robot_uptr->isSpotButtonPressed());
    _m_buttons_msg.wheeldrop = static_cast<int8_t>(_m_robot_uptr->isWheeldrop());
    _m_buttons_msg.cliff = static_cast<int8_t>(_m_robot_uptr->isCliff());
  }

  _m_zcm_udpm.publish("CRAEBOT_BUTTONS", &_m_buttons_msg);
}

}  // namespace driver
}  // namespace craebot
