cmake_minimum_required(VERSION 3.16)
project("ncdlgen-examples")

enable_language(CXX)

find_package(ncdlgen REQUIRED)

add_executable(custom_parser custom_parser.cpp)
target_link_libraries(custom_parser PRIVATE ncdlgen::ncdlgen)