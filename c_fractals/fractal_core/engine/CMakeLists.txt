add_library(sfml_engine
        engine.c
        engine.h
)


# 2. Expose this directory so 'app' can include "engine.h" easily
target_include_directories(sfml_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 3. Link SFML targets using modern namespaced targets
target_link_libraries(sfml_engine PRIVATE SDL3::SDL3)
