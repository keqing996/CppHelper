cmake_minimum_required(VERSION 3.22)

add_executable(test-string main.cpp)

target_link_libraries(test-string PRIVATE ${CPP_HELPER_LIB_NAME})