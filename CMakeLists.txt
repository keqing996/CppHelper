cmake_minimum_required(VERSION 3.22)

project(CppGeneralHelper)

set(CMAKE_CXX_STANDARD 20)
set(CPP_HELPER_LIB_NAME cpphelper)

# helper lib

option(CPP_HELPER_ENABLE_WINDOWS_OPENGL "Enable windows openGL ext" OFF)

file (GLOB_RECURSE HELPER_SRC src/Infra/*.cpp)
add_library(${CPP_HELPER_LIB_NAME} STATIC ${HELPER_SRC})
target_include_directories(${CPP_HELPER_LIB_NAME} PUBLIC src/)

if (NOT CPP_HELPER_ENABLE_WINDOWS_OPENGL)
    target_compile_definitions(${CPP_HELPER_LIB_NAME} PRIVATE DISABLE_HELPER_WINDOWS_OPENGL_SUPPORT)
endif ()

# test proj

option(CPP_HELPER_ENABLE_TEST "Enable test target" OFF)

if (CPP_HELPER_ENABLE_TEST)
    add_subdirectory(test)
endif ()
