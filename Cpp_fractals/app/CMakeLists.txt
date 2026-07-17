add_executable(FractalGenerator
        app.cc)

target_link_libraries(FractalGenerator PRIVATE sfml-graphics sfml-window sfml-system)
