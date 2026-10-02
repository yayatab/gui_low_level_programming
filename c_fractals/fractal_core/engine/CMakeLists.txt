add_library(sfml_engine
        engine.c
        thread_pool.c
        palette.c
)

target_include_directories(sfml_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sfml_engine PRIVATE SDL3::SDL3 viewport fractal)
