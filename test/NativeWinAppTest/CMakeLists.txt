add_executable          (test_window_style ./TestWindowStyle.cpp)
target_link_libraries   (test_window_style PRIVATE ${TARGET_LIB_NATIVE_WIN_APP})

add_executable          (test_window_opengl ./TestWindowOpenGL.cpp)
target_link_libraries   (test_window_opengl PRIVATE ${TARGET_LIB_NATIVE_WIN_APP})