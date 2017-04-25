set (EXE_NAME ${POD_NAME}-driver)

find_package (Boost REQUIRED COMPONENTS system thread)
include_directories (${Boost_INCLUDE_DIR})

set (REQUIRED_PACKAGES common lcm)
set (REQUIRED_LIBS ${Boost_LIBRARIES} create)

add_executable (${EXE_NAME} main.cpp driver.cpp)
target_link_libraries (${EXE_NAME} ${REQUIRED_LIBS})
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})
