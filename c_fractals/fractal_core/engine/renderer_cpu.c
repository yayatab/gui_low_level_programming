//
// Created by Yair Taboch on 12/08/2026.
//

#include "renderer_cpu.h"

#include <stdlib.h>

#pragma region Lifetime Management
void renderer_cpu_init(RendererCPU* renderer, SDL_Renderer* sdl_renderer, int width, int height){
  const uint32_t pitch = width * sizeof(uint32_t);
// TODO error handlers
  renderer->width = width;
  renderer->height = height;
  renderer->pixels = malloc(pitch * height);
  renderer->texture = SDL_CreateTexture(sdl_renderer,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
  renderer->pitch = pitch;
}

void renderer_cpu_resize(RendererCPU *renderer, SDL_Renderer *sdl_renderer, int new_width, int new_height) {
  renderer->width = new_width;
  renderer->height = new_height;
  renderer_cpu_destroy(renderer);
  const uint32_t pitch = new_width * sizeof(uint32_t);
  renderer->pixels = malloc(pitch * new_height);
  renderer->texture = SDL_CreateTexture(sdl_renderer,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, new_width, new_height);
}

void renderer_cpu_destroy(RendererCPU *renderer) {
  free(renderer->pixels);
  SDL_DestroyTexture(renderer->texture);
}

#pragma endregion Lifetime Management


#pragma region Buffer Manipulation Operations

void renderer_cpu_clear(RendererCPU *renderer, colour_t color) {

  for (int i = 0; i < renderer->width * renderer->height; i++) {
    renderer->pixels[i] = color;
  }
}

void renderer_cpu_set_pixel(const RendererCPU *renderer,const  int px,const  int py,const colour_t color) {
  if (px >= renderer->width || py >= renderer->height) {
    return;
  }
  renderer->pixels[py * renderer->width + px] = color;
}

void renderer_cpu_get_buffer(const RendererCPU *renderer, colour_t* out) {
 out = renderer->pixels;
}


#pragma endregion Buffer Manipulation Operations

#pragma region Display Output Pipeline

void renderer_cpu_present(RendererCPU *renderer, SDL_Renderer *sdl_renderer) {
  SDL_UpdateTexture(renderer->texture,NULL,renderer->pixels,renderer->pitch);

  SDL_RenderClear(sdl_renderer);
  SDL_RenderTexture(sdl_renderer, renderer->texture, NULL, NULL);
  SDL_RenderPresent(sdl_renderer);
}

#pragma endregion


#pragma region color
inline uint32_t create_colour(const int r,const int g,const int b,const int a){
  return  (r << 24) | (g << 16) | (b << 8) | a;
}

#pragma endregion