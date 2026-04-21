cmake_minimum_required(VERSION 3.22)
project(hollow_god LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(hollow_god
    src/main.cpp
)

target_include_directories(hollow_god PRIVATE src)

if(MSVC)
    target_compile_options(hollow_god PRIVATE /W4)
else()
    target_compile_options(hollow_god PRIVATE -Wall -Wextra -Wpedantic)
endif()
