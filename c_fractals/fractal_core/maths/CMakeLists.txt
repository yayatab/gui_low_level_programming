
add_library(
        fractal
        mandelbrot_set.c
        julia.c
        burning_ship.c)

target_include_directories(fractal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})