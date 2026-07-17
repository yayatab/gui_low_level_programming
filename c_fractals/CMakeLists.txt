include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.4.12 # Stable SDL2 release branch
        GIT_SHALLOW    ON
)

# Speed up compilation by turning off SDL components you don't need
#set(SDL_SHARED ON CACHE BOOL "Build shared SDL library" FORCE)
#set(SDL_STATIC OFF CACHE BOOL "Build static SDL library" FORCE)
#set(SDL_TEST OFF CACHE BOOL "Disable SDL test tools" FORCE)

FetchContent_MakeAvailable(SDL3)

add_subdirectory(fractal_core)
add_subdirectory(app)