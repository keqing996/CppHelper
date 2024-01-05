cmake_minimum_required(VERSION 3.26)

project(CppGeneralHelper)
set(CMAKE_CXX_STANDARD 20)

file (GLOB_RECURSE HELPER_SRC src/Helper/*.cpp)

# Test Win Window
add_executable(helper_test_win_window
        ${HELPER_SRC} tester/test_win_window.cpp)

target_include_directories(helper_test_win_window
        PRIVATE
        src/)

