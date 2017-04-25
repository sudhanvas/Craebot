#include "driver.hpp"

namespace craebot {
namespace driver {

sig_atomic_t CraebotDriver::_m_quit = false;
CraebotDriver* CraebotDriver::_m_this_instance = nullptr;

CraebotDriver::CraebotDriver(app_params_t& params)
    : _m_robot_ptr(nullptr)
    , _m_robot_model(create::RobotModel::CREATE_2)
    , _m_publish_sensor_data_thread(nullptr)
    , _m_publish_battery_state_thread (nullptr) {
  _m_this_instance = this;

  _init_msg();
  _init_signals();

  if (params.model_name == "roomba400") {
    _m_robot_model = create::RobotModel::ROOMBA_400;
  }
  else if (params.model_name == "create1") {
    _m_robot_model = create::RobotModel::CREATE_1;
  }
  else if (params.model_name == "create2") {
    _m_robot_model = create::RobotModel::CREATE_2;
  }
  else {
    fprintf(stderr, "The specified robot model name is not recognized.\n");
    exit (EXIT_FAILURE);
  }

  _m_robot_ptr = new create::Create(_m_robot_model);
  if (!_m_robot_ptr->connect(params.device_filename,
                             _m_robot_model.getBaud())) {
    fprintf(stderr, "Failed to establish a serial connection with Create.\n");
    exit (EXIT_FAILURE);
  }

  _m_robot_ptr->setMode(create::MODE_FULL);

  double current_battery_level = _m_robot_ptr->getBatteryCharge();
  current_battery_level /= _m_robot_ptr->getBatteryCapacity();
  current_battery_level *= 100.0;
  printf("Current battery percentage is %.2f%%\n", current_battery_level);

  long sensor_data_sleep_time =
      static_cast<long> (1.0/params.sensor_data_frequency * 1000.0);
   _m_sensor_data_sleep_ms = std::chrono::milliseconds(sensor_data_sleep_time);

  long battery_state_sleep_time =
      static_cast<long> (1.0/params.battery_state_frequency * 1000.0);
  _m_battery_state_sleep_ms =
      std::chrono::milliseconds(battery_state_sleep_time);
}

CraebotDriver::~CraebotDriver() {
  _cleanup();
}

void CraebotDriver::run(void) {
  _m_publish_sensor_data_thread =
      new std::thread(&CraebotDriver::_publish_sensor_data_thread, this);
  _m_publish_battery_state_thread =
      new std::thread(&CraebotDriver::_publish_battery_state_thread, this);

  _m_lcm.subscribe("CRAEBOT_TELEOP", &CraebotDriver::_teleop_callback, this);
  _m_lcm.subscribe("CRAEBOT_LEDS", &CraebotDriver::_leds_callback, this);
  _m_lcm.subscribe("CRAEBOT_ASCII", &CraebotDriver::_ascii_callback, this);

  while (!_m_lcm.handle());
}

void CraebotDriver::_init_msg(void) {
  _m_battery_state_msg.voltage = 0.0f;
  _m_battery_state_msg.current = 0.0f;
  _m_battery_state_msg.charge = 0.0f;
  _m_battery_state_msg.capacity = 0.0f;
  _m_battery_state_msg.charge_ratio = 0.0f;
  _m_battery_state_msg.temperature = 0;
  _m_battery_state_msg.charging_state = 0;

  _m_motor_feedback_msg.left_wheel_position = 0.0f;
  _m_motor_feedback_msg.right_wheel_position = 0.0f;
  _m_motor_feedback_msg.left_wheel_speed = 0.0f;
  _m_motor_feedback_msg.right_wheel_speed = 0.0f;

  _m_bumper_msg.is_left_pressed = false;
  _m_bumper_msg.is_right_pressed = false;
  _m_bumper_msg.is_light_left = false;
  _m_bumper_msg.is_light_front_left = false;
  _m_bumper_msg.is_light_center_left = false;
  _m_bumper_msg.is_light_right = false;
  _m_bumper_msg.is_light_front_right = false;
  _m_bumper_msg.is_light_center_right = false;

  _m_bumper_msg.light_signal_left = 0;
  _m_bumper_msg.light_signal_front_left = 0;
  _m_bumper_msg.light_signal_center_left = 0;
  _m_bumper_msg.light_signal_right = 0;
  _m_bumper_msg.light_signal_front_right = 0;
  _m_bumper_msg.light_signal_center_right = 0;
  _m_bumper_msg.omni_ir = 0;

  std::fill(_m_odometry_msg.pose, _m_odometry_msg.pose + 3, 0.0f);
  std::fill(_m_odometry_msg.velocity, _m_odometry_msg.velocity + 3, 0.0f);

  std::fill(_m_odometry_msg.pose_covariance,
            _m_odometry_msg.pose_covariance + 9,
            0.0f);
  std::fill(_m_odometry_msg.velocity_covariance,
            _m_odometry_msg.velocity_covariance + 9,
            0.0f);

  _m_buttons_msg.clean = false;
  _m_buttons_msg.day = false;
  _m_buttons_msg.hour = false;
  _m_buttons_msg.minute = false;
  _m_buttons_msg.dock = false;
  _m_buttons_msg.spot = false;
  _m_buttons_msg.wheeldrop = false;
  _m_buttons_msg.cliff = false;

  _m_last_buttons_msg.clean = false;
  _m_last_buttons_msg.day = false;
  _m_last_buttons_msg.hour = false;
  _m_last_buttons_msg.minute = false;
  _m_last_buttons_msg.dock = false;
  _m_last_buttons_msg.spot = false;
  _m_last_buttons_msg.wheeldrop = false;
  _m_last_buttons_msg.cliff = false;
}

void CraebotDriver::_init_signals(void) {
  _m_signal_action.sa_handler = _signal_handler;
  sigfillset(&_m_signal_action.sa_mask);
  _m_signal_action.sa_flags |= SA_SIGINFO;
  sigaction(SIGINT, &_m_signal_action, NULL);
}

void CraebotDriver::_cleanup() {
  _m_publish_sensor_data_thread->join();
  _m_publish_battery_state_thread->join();

  delete _m_publish_sensor_data_thread;
  delete _m_publish_battery_state_thread;
  delete _m_robot_ptr;
}

void CraebotDriver::_teleop_callback(
    const lcm::ReceiveBuffer *rbuf,
    const std::string &channel,
    const teleop_t *teleop) {
  std::lock_guard<std::mutex> guard(_m_robot_mutex);
  _m_robot_ptr->drive(teleop->linear_velocity, teleop->angular_velocity);
}

void CraebotDriver::_leds_callback(
    const lcm::ReceiveBuffer *rbuf,
    const std::string &channel,
    const leds_t *leds) {
  std::lock_guard<std::mutex> guard(_m_robot_mutex);
  _m_robot_ptr->enableDebrisLED(static_cast<bool> (leds->debris));
  _m_robot_ptr->enableSpotLED(static_cast<bool> (leds->spot));
  _m_robot_ptr->enableDockLED(static_cast<bool> (leds->dock));
  _m_robot_ptr->enableCheckRobotLED(static_cast<bool> (leds->check));
  _m_robot_ptr->setPowerLED(leds->power[0], leds->power[1]);
}

void CraebotDriver::_ascii_callback(
    const lcm::ReceiveBuffer *rbuf,
    const std::string &channel,
    const ascii_t *ascii) {
  bool result = true;

  switch (ascii->nCharacters) {
    case 0:
      fprintf(stderr, "No ASCII characters provided.\n");
      break;
    case 1:
      result = _m_robot_ptr->setDigitsASCII(ascii->data[0], ' ', ' ', ' ');
      break;
    case 2:
      result = _m_robot_ptr->setDigitsASCII(
          ascii->data[0],
          ascii->data[1],
          ' ',
          ' ');
      break;
    case 3:
      result = _m_robot_ptr->setDigitsASCII(
          ascii->data[0],
          ascii->data[1],
          ascii->data[2],
          ' ');
        break;
    case 4:
      result = _m_robot_ptr->setDigitsASCII(
          ascii->data[0],
          ascii->data[1],
          ascii->data[2],
          ascii->data[3]);
      break;
    default:
      result = false;
      break;
  }

  if (!result) {
    fprintf(stderr, "ASCII character out of range [32, 126].\n");
  }
}

void CraebotDriver::_publish_sensor_data_thread(void) {
  while (!_m_quit) {
    _publish_motor_feedback();
    _publish_bumper_data();
    _publish_odometry_data();
    _publish_button_states();
    std::this_thread::sleep_for(_m_sensor_data_sleep_ms);
  }
}

void CraebotDriver::_publish_battery_state_thread(void) {
  while (!_m_quit) {
    _m_battery_state_msg.utime = common::now();

    {
      std::lock_guard<std::mutex> guard(_m_robot_mutex);
      _m_battery_state_msg.voltage = _m_robot_ptr->getVoltage();
      _m_battery_state_msg.current = _m_robot_ptr->getCurrent();
      _m_battery_state_msg.charge = _m_robot_ptr->getBatteryCharge();
      _m_battery_state_msg.capacity = _m_robot_ptr->getBatteryCapacity();
      _m_battery_state_msg.temperature = _m_robot_ptr->getTemperature();
      _m_battery_state_msg.charge_ratio = _m_battery_state_msg.charge;
      _m_battery_state_msg.charge_ratio /= _m_battery_state_msg.capacity;
      _m_battery_state_msg.charging_state = static_cast<uint8_t>
          (_m_robot_ptr->getChargingState());
    }

    _m_lcm.publish("CRAEBOT_BATTERY_STATE", &_m_battery_state_msg);
    std::this_thread::sleep_for(_m_battery_state_sleep_ms);
  }
}

void CraebotDriver::_publish_motor_feedback(void) {
  _m_motor_feedback_msg.utime = common::now();

  float wheel_radius = _m_robot_model.getWheelDiameter() / 2.0f;

  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);
    _m_motor_feedback_msg.left_wheel_position =
        _m_robot_ptr->getLeftWheelDistance() / wheel_radius;
    _m_motor_feedback_msg.right_wheel_position =
        _m_robot_ptr->getRightWheelDistance() / wheel_radius;
    _m_motor_feedback_msg.left_wheel_speed =
        _m_robot_ptr->getRequestedLeftWheelVel() / wheel_radius;
    _m_motor_feedback_msg.right_wheel_speed =
        _m_robot_ptr->getRequestedRightWheelVel() / wheel_radius;
  }

  _m_lcm.publish("CRAEBOT_MOTOR_FEEDBACK", &_m_motor_feedback_msg);
}

void CraebotDriver::_publish_bumper_data() {
  _m_bumper_msg.utime = common::now();

  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);

    _m_bumper_msg.is_left_pressed = _m_robot_ptr->isLeftBumper();
    _m_bumper_msg.is_right_pressed = _m_robot_ptr->isRightBumper();

    if (_m_robot_model.getVersion() >= create::V_3)
    {
      _m_bumper_msg.is_light_left =
          _m_robot_ptr->isLightBumperLeft();
      _m_bumper_msg.is_light_front_left =
          _m_robot_ptr->isLightBumperFrontLeft();
      _m_bumper_msg.is_light_center_left =
          _m_robot_ptr->isLightBumperCenterLeft();
      _m_bumper_msg.is_light_right =
          _m_robot_ptr->isLightBumperRight();
      _m_bumper_msg.is_light_front_right =
          _m_robot_ptr->isLightBumperFrontRight();
      _m_bumper_msg.is_light_center_right =
          _m_robot_ptr->isLightBumperCenterRight();

      _m_bumper_msg.light_signal_left =
          _m_robot_ptr->getLightSignalLeft();
      _m_bumper_msg.light_signal_front_left =
          _m_robot_ptr->getLightSignalFrontLeft();
      _m_bumper_msg.light_signal_center_left =
          _m_robot_ptr->getLightSignalCenterLeft();
      _m_bumper_msg.light_signal_right =
          _m_robot_ptr->getLightSignalRight();
      _m_bumper_msg.light_signal_front_right =
          _m_robot_ptr->getLightSignalFrontRight();
      _m_bumper_msg.light_signal_center_right =
          _m_robot_ptr->getLightSignalCenterRight();

      _m_bumper_msg.omni_ir = _m_robot_ptr->getIROmni();
    }
  }

  _m_lcm.publish("CRAEBOT_BUMPER", &_m_bumper_msg);
}

void CraebotDriver::_publish_odometry_data(void) {
  _m_odometry_msg.utime = common::now();

  create::Pose pose;
  create::Vel velocity;

  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);
    pose = _m_robot_ptr->getPose();
    velocity = _m_robot_ptr->getVel();
  }

  _m_odometry_msg.pose[0] = pose.x;
  _m_odometry_msg.pose[1] = pose.y;
  _m_odometry_msg.pose[2] = pose.yaw;

  _m_odometry_msg.velocity[0] = velocity.x;
  _m_odometry_msg.velocity[1] = velocity.y;
  _m_odometry_msg.velocity[2] = velocity.yaw;

  for (uint8_t i = 0; i < 9; ++i) {
    _m_odometry_msg.pose_covariance[i] = pose.covariance[i];
    _m_odometry_msg.velocity_covariance[i] = velocity.covariance[i];
  }

  _m_lcm.publish("CRAEBOT_ODOMETRY", &_m_odometry_msg);
}

void CraebotDriver::_publish_button_states(void) {
  {
    std::lock_guard<std::mutex> guard(_m_robot_mutex);
    _m_buttons_msg.clean = _m_robot_ptr->isCleanButtonPressed();
    _m_buttons_msg.day = _m_robot_ptr->isDayButtonPressed();
    _m_buttons_msg.hour = _m_robot_ptr->isHourButtonPressed();
    _m_buttons_msg.minute = _m_robot_ptr->isMinButtonPressed();
    _m_buttons_msg.dock = _m_robot_ptr->isDockButtonPressed();
    _m_buttons_msg.spot = _m_robot_ptr->isSpotButtonPressed();
    _m_buttons_msg.wheeldrop = _m_robot_ptr->isWheeldrop();
    _m_buttons_msg.cliff = _m_robot_ptr->isCliff();
  }

  if (_m_buttons_msg != _m_last_buttons_msg) {
    _m_buttons_msg.utime = common::now();
    _m_lcm.publish("CRAEBOT_BUTTONS", &_m_buttons_msg);
    _m_last_buttons_msg = _m_buttons_msg;
  }
}

inline void CraebotDriver::_signal_handler(int signal) {
  if (signal == SIGINT) {
    printf("Goodbye!\n");
    _m_quit = true;
    _m_this_instance->_cleanup();
    std::exit (EXIT_SUCCESS);
  }
}

bool operator!= (const buttons_t& lhs, const buttons_t& rhs) {
  return !(
      lhs.clean == rhs.clean and
      lhs.day == rhs.day and
      lhs.hour == rhs.hour and
      lhs.minute == rhs.minute and
      lhs.dock == rhs.dock and
      lhs.spot == rhs.spot and
      lhs.wheeldrop == rhs.wheeldrop and
      lhs.cliff == rhs.wheeldrop);
}

} // namespace driver
} // namespace craebot
