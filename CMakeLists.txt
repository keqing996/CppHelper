cmake_minimum_required(VERSION 3.22)

project (CppInfra)
set (CMAKE_CXX_STANDARD 20)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/artifacts/)

# Names
set (TARGET_LIB_NATIVE_WIN_APP cpp_native_win_app)

# Source files
file (GLOB_RECURSE NATIVE_WIN_WINDOW_SRC ./src/NativeWinApp/*.cpp)

# Native win app
add_library                 (${TARGET_LIB_NATIVE_WIN_APP} STATIC ${NATIVE_WIN_WINDOW_SRC})
target_include_directories  (${TARGET_LIB_NATIVE_WIN_APP} PUBLIC ./include/)

# Test code
add_subdirectory (test)