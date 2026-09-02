cmake_minimum_required(VERSION 3.20)
project(Raycaster CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Raycaster main.cpp Engine.cpp)
target_link_libraries(Raycaster PRIVATE ${SDL2_LIBRARIES})