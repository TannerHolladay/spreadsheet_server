cmake_minimum_required(VERSION 3.15)
project(spreadsheet_server)

set(CMAKE_CXX_STANDARD 98)

add_executable(spreadsheet_server main.cpp client.cpp cell.cpp spreadsheet.cpp)