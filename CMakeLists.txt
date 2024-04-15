cmake_minimum_required(VERSION 3.22)

project(CppGeneralHelper)

set(CMAKE_CXX_STANDARD 20)
set(CPP_HELPER_LIB_NAME cpphelper)

# helper lib
file (GLOB_RECURSE HELPER_SRC src/Infra/*.cpp)
add_library(${CPP_HELPER_LIB_NAME} STATIC ${HELPER_SRC})
target_include_directories(${CPP_HELPER_LIB_NAME} PUBLIC src/)

# test proj
add_subdirectory(test)
