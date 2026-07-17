cmake_minimum_required(VERSION 4.1)
project(fractal_practice LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

add_subdirectory(Cpp_fractals)
add_subdirectory(c_fractals)
add_subdirectory(introduction_to_c_and_gui_programming)