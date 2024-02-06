cmake_minimum_required(VERSION 3.22)

project(CppGeneralHelper)

set(CMAKE_CXX_STANDARD 20)
set(CPP_HELPER_LIB_NAME cpphelper)

# helper lib
file (GLOB_RECURSE HELPER_SRC src/Helper/*.cpp)
add_library(${CPP_HELPER_LIB_NAME} STATIC ${HELPER_SRC})
target_include_directories(${CPP_HELPER_LIB_NAME} PUBLIC src/)

# test proj

option(CPP_HELPER_ENABLE_TEST "Enable test target" OFF)

if (CPP_HELPER_ENABLE_TEST)
    add_subdirectory(test)
endif ()

# Test Win Window
#add_executable(helper_test_win_window
#        ${HELPER_SRC} test/test-win-window/test_win_window.cpp)
#
#target_compile_definitions(helper_test_win_window
#        PRIVATE
#        ENABLE_HELPER_EXT_OPENGL
#)
#
#target_include_directories(helper_test_win_window
#        PRIVATE
#        src/)
#
#target_include_directories(helper_test_win_window
#        PRIVATE
#        "${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/")
