include(ExternalProject)

# ============================================================== #
# Qt5
# ============================================================== #
find_package(Qt5 5.9 REQUIRED COMPONENTS Core Widgets Gui Charts Concurrent Network LinguistTools PATHS /opt/Qt/5.9.1/gcc_64/ /home/Qt/5.9.1/gcc_64/)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)   # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON)               # Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTORCC ON)               # Instruct CMake to run rcc automatically when needed.
set(CMAKE_AUTOUIC ON)               # Instruct CMake to run uic automatically when needed.

if (NOT NoDesigner)
    find_package(Qt5 5.9 REQUIRED COMPONENTS Designer PATHS /opt/Qt/5.9.1/gcc_64/ /home/Qt/5.9.1/gcc_64/)
endif ()

# ============================================================== #
# Catch
# ============================================================== #
if (TESTS)
    set(CATCH_VERSION "2.1.1")
    ExternalProject_Add(
        BuildCatch
        PREFIX ${CMAKE_BINARY_DIR}/catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v${CATCH_VERSION}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/contrib
        LOG_DOWNLOAD 1
        UPDATE_DISCONNECTED 1
    )
    add_library(Catch INTERFACE)
    add_dependencies(Catch BuildCatch)
    target_include_directories(Catch INTERFACE ${CMAKE_BINARY_DIR}/contrib/include/catch/)
endif()
