find_package (
        Boost REQUIRED COMPONENTS
        filesystem
        program_options
        system
        thread)
include_directories (${Boost_INCLUDE_DIR})

set (EXE_NAME ${POD_NAME}-driver)
set (REQUIRED_PACKAGES zcm)
set (REQUIRED_LIBS ${Boost_LIBRARIES} create pthread)

file (GLOB SOURCE_FILES *.cc)
add_executable (${EXE_NAME} ${SOURCE_FILES})
target_link_libraries (${EXE_NAME} ${REQUIRED_LIBS})
pods_use_pkg_config_packages (${EXE_NAME} ${REQUIRED_PACKAGES})
pods_install_executables (${EXE_NAME})
