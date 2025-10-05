# BASE CMAKE
cmake_minimum_required(VERSION 3.31.6)
project(WavePDF VERSION 0.1)

# C++ STANDARD
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# CONAN TOOLCHAIN
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# EXECUTABLE
add_executable(WavePDF src/main.cpp)

# wxWidgets aus Conan verwenden
target_link_libraries(WavePDF PRIVATE wxWidgets::wxWidgets)