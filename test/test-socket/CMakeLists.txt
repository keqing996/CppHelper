cmake_minimum_required(VERSION 3.22)

add_executable(test-socket main.cpp)

target_link_libraries(test-socket PRIVATE ${CPP_HELPER_LIB_NAME})