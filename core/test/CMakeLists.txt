
project(novelist_core_test)

enable_testing()

set(SOURCE_FILES main.cpp main.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} novelist_core Qt5::Test)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic")