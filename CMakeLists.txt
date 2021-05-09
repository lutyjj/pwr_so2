cmake_minimum_required(VERSION 3.10)

project(SO2)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})  

find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)

include_directories(include)

add_executable(main src/main.cpp src/Road.cpp src/Car.cpp)
target_link_libraries(main ${CURSES_LIBRARIES} Threads::Threads)