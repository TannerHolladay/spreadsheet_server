cmake_minimum_required(VERSION 3.16)
project(spreadsheet_server)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
	  googletest
	    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Build the code
enable_testing()

add_executable(
    tests
    tests.cc
    cell.cpp
    client.cpp
    spreadsheet.cpp
)

target_link_libraries(
    tests
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)

