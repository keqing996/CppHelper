
include_directories(../../src/)

# test
add_executable(test_infra_socket ./TestSocket.cpp)
add_executable(test_infra_string ./TestString.cpp)
add_executable(test_infra_console ./TestConsole.cpp)
add_executable(test_infra_process ./TestProcess.cpp)