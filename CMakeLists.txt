cmake_minimum_required(VERSION 3.15)
project(1brc-cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_executable(${PROJECT_NAME}
    src/generator.cpp
)

add_executable(solution 
    src/main.cpp
)

target_include_directories(solution
    PUBLIC
    $<INSTALL_INTERFACE:>/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
)