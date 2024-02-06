cmake_minimum_required(VERSION 3.22)

add_executable(test-win-window main.cpp)

target_link_libraries(test-win-window PRIVATE ${CPP_HELPER_LIB_NAME})