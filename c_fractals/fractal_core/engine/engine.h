#pragma once
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

typedef struct {
  int height;
  int width;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint32_t *pixel_buffer;
  bool is_running;
} Engine;


int engine_init(Engine *engine,  int height, int width);

int engine_run(Engine *engine);

int engine_update(const Engine *engine);

int engine_handle_events(Engine *engine);

int engine_load_func(Engine *engine, void* func);

int engine_stop(Engine *engine);

/**
 * Cleanup the engine data
 */
int engine_cleanup(Engine *engine);