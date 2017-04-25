set (REQUIRED_PACKAGES common lcm)

set (EXE_NAME ${POD_NAME}-test-ascii)
add_executable (${EXE_NAME} ascii_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})

set (EXE_NAME ${POD_NAME}-test-battery-state)
add_executable (${EXE_NAME} battery_state_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})

set (EXE_NAME ${POD_NAME}-test-bumper)
add_executable (${EXE_NAME} bumpers_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})

set (EXE_NAME ${POD_NAME}-test-buttons)
add_executable (${EXE_NAME} buttons_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})

set (EXE_NAME ${POD_NAME}-test-leds)
add_executable (${EXE_NAME} leds_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})

set (EXE_NAME ${POD_NAME}-test-motor-feedback)
add_executable (${EXE_NAME} motor_feedback_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})

set (EXE_NAME ${POD_NAME}-test-odometry)
add_executable (${EXE_NAME} odometry_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})

set (EXE_NAME ${POD_NAME}-test-teleop)
add_executable (${EXE_NAME} teleop_test.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})
