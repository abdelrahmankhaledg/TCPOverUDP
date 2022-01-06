cmake_minimum_required(VERSION 3.21)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled main.cpp Packet.h cwnd.h Ack_packet.h slow_start.h serialize_deserialize.h ThreadFunction.cpp ThreadFunction.h Server.cpp Client.cpp)
