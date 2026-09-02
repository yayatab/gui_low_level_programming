add_library(sfml_engine
        engine.c
        engine.h
)

target_include_directories(sfml_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sfml_engine PRIVATE SDL3::SDL3)

add_library(renderer_cpu
        renderer_cpu.c
        renderer_cpu.h
)

target_include_directories(renderer_cpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(renderer_cpu PRIVATE SDL3::SDL3)

add_library(engine_utils
        engine_utils.c
        engine_utils.h
)

target_include_directories(engine_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine_utils PRIVATE SDL3::SDL3)
