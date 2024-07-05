# Minimum version of CMake required
cmake_minimum_required(VERSION 3.16)

# Project name and the language used
project(confltr VERSION 1.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)
    include_directories("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.39.33519/include")
endif()

# Create libraries directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries)
set(ARGPARSE_HEADER ${CMAKE_SOURCE_DIR}/libraries/argparse.hpp)
if(NOT EXISTS ${ARGPARSE_HEADER})
    file(DOWNLOAD
        https://raw.githubusercontent.com/p-ranav/argparse/master/include/argparse/argparse.hpp
        ${ARGPARSE_HEADER}
    )
endif()

include_directories(${CMAKE_SOURCE_DIR}/libraries)

# Add executable
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/**/*.cpp" "src/**/**/*.cpp")
add_executable(confltr ${SOURCES})

# Link libraries
target_link_libraries(confltr PRIVATE )