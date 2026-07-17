add_executable(fractal_app main.c)

# 2. Link your engine (CMake automatically resolves SFML dependencies)
target_link_libraries(fractal_app PRIVATE SDL3::SDL3)