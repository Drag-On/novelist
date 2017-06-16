cmake_minimum_required(VERSION 3.7)
project(novelist)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

include(cmake/Options.cmake)
include(cmake/Util.cmake)
include(cmake/Dependencies.cmake)
include(cmake/Summary.cmake)

add_subdirectory(core)
#add_subdirectory(launcher)
#add_subdirectory(plugins)

# Copy default settings to binary dir
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/settings DESTINATION ${CMAKE_BINARY_DIR}/) # TODO: Enable this once there are any

# Print configuration summary
print_config_summary()