cmake_minimum_required(VERSION 3.21)
project(untitled1)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled1 Packet.h Ack_packet.h Client.cpp serialize_deserialize.h serialize_deserialize.cpp)
