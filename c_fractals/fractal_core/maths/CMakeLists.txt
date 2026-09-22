
add_library(
        fractal
        fractal.h
        mandelbrot_set.c
        mandelbrot_set.h)

target_include_directories(fractal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})