#pragma once
#include "thread_pool.h"
#include "viewport.h"
#include "fractal.h"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"


typedef struct {
  int height;
  int width;
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  uint32_t* pixel_buffer;
  bool is_running;
  size_t thread_count;
  ThreadPool thread_pool;
  int palette_index;
  int julia_index;
  int fractal_index;
  float* iteration_buffer;
} Engine;


int engine_init(Engine* engine, const int height, const int width);

int engine_init_mt(Engine* engine, int height, int width, size_t thread_count);

void engine_parallel_for(Engine* engine, int start, int end, ParallelForFunc func, void* user_data);

int engine_update(const Engine* engine);

int engine_handle_events(Engine* engine, Viewport* vp);

void engine_change_set(Engine* engine);


/**
 * Cleanup the engine data
 */
int engine_cleanup(Engine* engine);