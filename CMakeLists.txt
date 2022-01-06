cmake_minimum_required(VERSION 3.21)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

add_executable(untitled Packet.h cwnd.h Ack_packet.h slow_start.h serialize_deserialize.h ThreadFunction.cpp ThreadFunction.h Server.cpp cwnd.cpp serialize_deserialize.cpp)

target_link_libraries( untitled ${CMAKE_THREAD_LIBS_INIT} )