set (LIB_NAME common)

file (GLOB HEADER_FILES *.hh)

pods_install_headers (${HEADER_FILES} DESTINATION ${LIB_NAME})
pods_install_pkg_config_file(
        ${LIB_NAME}
        LIBS -l${LIB_NAME}
        VERSION 1.0)
