cmake_minimum_required(VERSION 3.22)

project(CppInfra)
set(CMAKE_CXX_STANDARD 20)

include_directories(./include/)

# test
add_executable(test_socket ./test/TestSocket.cpp)
add_executable(test_string ./test/TestString.cpp)