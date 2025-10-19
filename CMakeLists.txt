# BASE CMAKE
cmake_minimum_required(VERSION 3.15)
project(WavePDF VERSION 0.1)

# C++ STANDARD
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# CONAN TOOLCHAIN
include(${CMAKE_BINARY_DIR}/conan/conan_toolchain.cmake)

# CONAN DEPENDENCIES
include(${CMAKE_BINARY_DIR}/conan/conandeps.cmake OPTIONAL)
include(${CMAKE_BINARY_DIR}/conan/conandeps_legacy.cmake OPTIONAL)

# SOURCE FILES
file(GLOB_RECURSE SOURCES
    src/**/*.cpp
)

# SET EXECUTABLE
add_executable(WavePDF 
    src/main.cpp
    ${SOURCES}
)

# USE wxWidgets FROM CONAN
target_link_libraries(WavePDF PRIVATE wxWidgets::wxWidgets)

# SET INCLUDE PATH
target_include_directories(WavePDF PRIVATE src) 

# TESTING
enable_testing()
add_executable(test_pdfReader 
    tests/test_pdfreader.cpp
    ${SOURCES}
)
target_link_libraries(test_pdfReader PRIVATE gtest::gtest wxWidgets::wxWidgets)
add_test(NAME PdfReaderTest COMMAND test_pdfReader)
