set (EXE_NAME ${POD_NAME}-teleop)
set (REQUIRED_PACKAGES common lcm)

add_executable (${EXE_NAME} main.cpp teleop.cpp)
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})
