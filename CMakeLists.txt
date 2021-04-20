cmake_minimum_required(VERSION 3.10)
project(comm_mgr)

ADD_EXECUTABLE( comm_mgr main.c uart.c )
INCLUDE_DIRECTORIES( include )
target_link_libraries(comm_mgr pthread)