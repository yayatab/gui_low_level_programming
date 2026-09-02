#pragma once

/*
* Key Responsibilities of the Renderer Module
* Pixel Buffer Ownership: Allocate and manage a linear array of 32-bit pixel colors (uint32_t *pixels).
* SDL3 Texture Management: Create an SDL_Texture configured for streaming (SDL_TEXTUREACCESS_STREAMING) with SDL_PIXELFORMAT_RGBA8888 (or SDL_PIXELFORMAT_ARGB8888).
* Buffer Upload & Present: Copy raw CPU memory directly to the GPU texture frame-by-frame and draw it to the window via SDL_RenderTexture.
*/

#include <SDL3/SDL.h>
#include <stdint.h>

typedef uint32_t colour_t;

// Core CPU Renderer Instance
typedef struct {
  uint32_t *pixels;        // 32-bit linear RGBA pixel array (Width * Height)
  SDL_Texture *texture;    // SDL3 streaming texture target
  int width;               // Buffer width in pixels
  int height;              // Buffer height in pixels
  size_t pitch;            // Row stride in bytes (width * sizeof(uint32_t))
} RendererCPU;

#pragma region Lifetime Management

/**
 * Allocates the raw CPU pixel buffer (uint32_t) and creates an SDL_Texture set to
 * SDL_TEXTUREACCESS_STREAMING with format SDL_PIXELFORMAT_RGBA8888.
 *
 * @param renderer
 * @param sdl_renderer
 * @param width
 * @param height
 */
void renderer_cpu_init(RendererCPU *renderer, SDL_Renderer *sdl_renderer, int width, int height);

/**
 * Reallocates the pixel memory array and destroys/recreates the streaming texture when
 * the window is resized
 *
 * @param renderer
 * @param sdl_renderer
 * @param new_width
 * @param new_height
 */
void renderer_cpu_resize(RendererCPU *renderer, SDL_Renderer *sdl_renderer, int new_width, int new_height);

/**
 * Frees heap pixel memory and destroys the SDL_Texture
 * @param renderer
 */
void renderer_cpu_destroy(RendererCPU *renderer);

#pragma endregion

#pragma region Buffer Manipulation Operations

/**
 * Sets every pixel in the CPU buffer to a single background color (e.g., solid black or transparent).
 *
 * @param renderer
 * @param color
 */
void renderer_cpu_clear(RendererCPU *renderer, uint32_t color);

/**
 * Writes a single pixel value with bounds checking.
 * Essential for non-grid drawing (like axis lines or UI overlays).
 *
 * @param renderer
 * @param px
 * @param py
 * @param color
 */
void renderer_cpu_set_pixel(const RendererCPU *renderer, int px, int py, colour_t color);

/**
 * Exposes the raw uint32_t* pointer for direct hot-path batch writes during mathematical
 * iteration loops (or passing to multithreaded workers).
 *
 * @param renderer
 * @param out
 */
void renderer_cpu_get_buffer(const RendererCPU *renderer, uint32_t* out);

#pragma endregion

#pragma region Display Output Pipeline

/**
 * Uploads the raw CPU pixel buffer into the SDL3 streaming texture (SDL_UpdateTexture)
 * and renders it to the screen via SDL_RenderTexture
 *
 * @param renderer
 * @param sdl_renderer
 */
void renderer_cpu_present(RendererCPU *renderer, SDL_Renderer *sdl_renderer);

#pragma endregion


#pragma region color
colour_t create_colour(int r, int g, int b, int a);

#pragma  endregion