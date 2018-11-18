find_package (Boost REQUIRED COMPONENTS program_options)
include_directories (${Boost_INCLUDE_DIR})

set (EXE_NAME ${POD_NAME}-teleop)
set (REQUIRED_PACKAGES zcm)
set (REQUIRED_LIBS ${Boost_LIBRARIES} pthread)

add_executable (${EXE_NAME} main.cc teleop.cc)
target_link_libraries (${EXE_NAME} ${REQUIRED_LIBS})
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})
