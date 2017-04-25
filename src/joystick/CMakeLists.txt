set (EXE_NAME ${POD_NAME}-joystick)

set (REQUIRED_PACKAGES common lcm)

add_executable (${EXE_NAME} main.cpp joystick.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})
