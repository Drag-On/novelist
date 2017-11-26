cmake_minimum_required(VERSION 3.8)
project(novelist)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

include(GenerateExportHeader)
include(cmake/Options.cmake)
include(cmake/Util.cmake)
include(cmake/Dependencies.cmake)
include(cmake/Summary.cmake)

add_subdirectory(lib)
add_subdirectory(core)
add_subdirectory(plugins)
add_subdirectory(launcher)

# Copy default settings to binary dir
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/settings DESTINATION ${CMAKE_BINARY_DIR}/) # TODO: Enable this once there are any

# Print configuration summary
print_config_summary()