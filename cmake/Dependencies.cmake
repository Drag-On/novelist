
# ============================================================== #
# Qt5
# ============================================================== #
find_package(Qt5 5.9 REQUIRED COMPONENTS Core Widgets Gui Test Concurrent Network LinguistTools PATHS /opt/Qt/5.9.1/gcc_64/ /home/Qt/5.9.1/gcc_64/)
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
find_package(Catch)
