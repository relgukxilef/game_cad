cmake_minimum_required(VERSION 3.5)

project(GameCAD CXX)

enable_testing()

add_subdirectory(source)
target_include_directories(GameCAD PUBLIC include)

add_subdirectory(samples)
