cmake_minimum_required(VERSION 3.22)

project (CppInfra)
set (CMAKE_CXX_STANDARD 20)

# Set output path
# set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/artifacts/)

# Names
set (TARGET_LIB_NATIVE_WIN_APP  cpp_native_win_app)
set (TARGET_LIB_IMGUI_WIN_APP   imgui_frame_work)

# Source files
file (GLOB_RECURSE NATIVE_WIN_WINDOW_SRC ./src/NativeWinApp/*.cpp)
file (GLOB_RECURSE IMGUI_WIN_WINDOW_SRC ./src/ImGuiWinApp/*.cpp)

# Native win app
add_library                 (${TARGET_LIB_NATIVE_WIN_APP} STATIC ${NATIVE_WIN_WINDOW_SRC})
target_include_directories  (${TARGET_LIB_NATIVE_WIN_APP} PUBLIC ./src/)

# ImGui win app
add_library                 (${TARGET_LIB_IMGUI_WIN_APP} STATIC ${NATIVE_WIN_WINDOW_SRC} ${IMGUI_WIN_WINDOW_SRC})
target_include_directories  (${TARGET_LIB_IMGUI_WIN_APP} PUBLIC ./src/)
target_include_directories  (${TARGET_LIB_IMGUI_WIN_APP} PUBLIC ./src/ImGuiWinApp/ImGui/)

# Test code
option (ENABLE_INFRA_TEST OFF)

if (ENABLE_INFRA_TEST)
    add_subdirectory (test)
endif ()