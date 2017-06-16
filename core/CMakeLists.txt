
project(novelist_core)

set(SOURCE_FILES )
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic")

add_subdirectory(test)