cmake_minimum_required(VERSION 3.22)

project (CppInfra)
set (CMAKE_CXX_STANDARD 20)

# Set output path
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/artifacts/${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE}/)

# Source files
file (GLOB_RECURSE INFRA_SRC ./src/*.cpp)

# Native win app
add_library                 (infra STATIC ${INFRA_SRC})
target_include_directories  (infra PUBLIC ./include/)

# Test code
option (ENABLE_INFRA_TEST OFF)

if (ENABLE_INFRA_TEST)
    add_executable          (test_socket ./test/TestSocket.cpp)
    target_link_libraries   (test_socket infra)

    add_executable          (test_string ./test/TestString.cpp)
    target_link_libraries   (test_string infra)

    add_executable          (test_console ./test/TestConsole.cpp)
    target_link_libraries   (test_console infra)

    add_executable          (test_process ./test/TestProcess.cpp)
    target_link_libraries   (test_process infra)

    add_executable          (test_system ./test/TestSystem.cpp)
    target_link_libraries   (test_system infra)

    add_executable          (test_command_line ./test/TestCommandLine.cpp)
    target_link_libraries   (test_command_line infra)
endif ()