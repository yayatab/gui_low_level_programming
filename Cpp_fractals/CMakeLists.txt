include(FetchContent)

#FetchContent_Declare(
#        SFML
#        GIT_REPOSITORY https://github.com/SFML/SFML.git
#        GIT_TAG 3.0.2
#        GIT_SHALLOW ON
#        EXCLUDE_FROM_ALL
#        SYSTEM
#)
#
#FetchContent_MakeAvailable(SFML)

add_executable(fractal_check main.cc
        fractal_core/fractal_engine.cc
        fractal_core/fractal_engine.h)

target_link_libraries(fractal_check PRIVATE sfml-graphics sfml-window sfml-system)

add_subdirectory(app)
add_subdirectory(fractal_core)
add_subdirectory(renderer_cpu)

