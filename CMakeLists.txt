cmake_minimum_required(VERSION 3.16)
project(spreadsheet_server)

set(CMAKE_CXX_STANDARD 98)

add_executable(spreadsheet_server main.cpp cell.cpp client.cpp spreadsheet.cpp)
