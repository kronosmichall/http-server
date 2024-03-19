cmake_minimum_required(VERSION 3.10)
project(httpserver)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pthread")

include_directories(src/ph)

file(GLOB SOURCES "src/*.c" "src/ph/*.c")

add_executable(main ${SOURCES})
