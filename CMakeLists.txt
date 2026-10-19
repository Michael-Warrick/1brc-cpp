cmake_minimum_required(VERSION 3.15)
project(1brc-cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -Wall -Wextra -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-march=native -O3 -DNDEBUG")

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