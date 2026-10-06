add_executable(fractal_app main.c)

target_link_libraries(fractal_app PRIVATE SDL3::SDL3 sfml_engine viewport fractal)