set (LIB_NAME common)

file (GLOB HEADER_FILES *.hpp)
file (GLOB SOURCE_FILES *.cpp)

add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

pods_install_libraries(${LIB_NAME})
pods_install_headers (${HEADER_FILES} DESTINATION ${LIB_NAME})
pods_install_pkg_config_file(${LIB_NAME}
        LIBS -l${LIB_NAME} ${REQUIRED_LIBS}
        REQUIRES ${REQUIRED_PACAKGES}
        VERSION 0.0.1)
