#include "engine.h"

#include <stdlib.h>

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"

#include "thread_pool.h"


int engine_init(Engine* engine, const int height, const int width) {
  return engine_init_mt(engine, height, width, 1);
}

int engine_init_mt(Engine* engine, const int height, const int width, const size_t thread_count) {
  engine->width = width;
  engine->height = height;
  engine->thread_count = thread_count;
  engine->palette_index = 0;
  engine->is_running = false;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Unable to initialize SDL3: %s", SDL_GetError());
    return 1;
  }

  if (!SDL_CreateWindowAndRenderer("SDL3 Pure C Fractal Platform", width, height, 0, &engine->window,
                                   &engine->renderer)) {
    SDL_Log("Could not create window/renderer: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  engine->texture = SDL_CreateTexture(engine->renderer, SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_STREAMING, width, height);

  engine->pixel_buffer = malloc(width * height * sizeof(uint32_t));

  if (!engine->pixel_buffer) {
    SDL_Log("Failed to allocate heap pixel buffer");
    SDL_DestroyTexture(engine->texture);
    SDL_DestroyRenderer(engine->renderer);
    SDL_DestroyWindow(engine->window);
    SDL_Quit();
    return 1;
  }
  thread_pool_init(&engine->thread_pool, engine->thread_count);
  engine->is_running = true;

  return 0;
}

int engine_update(const Engine* engine) {
  if (!engine->is_running) {
    return 1;
  }

  SDL_UpdateTexture(engine->texture, NULL, engine->pixel_buffer, engine->width * sizeof(uint32_t));

  SDL_RenderClear(engine->renderer);
  SDL_RenderTexture(engine->renderer, engine->texture, NULL, NULL);
  SDL_RenderPresent(engine->renderer);
  return 0;
}

int engine_handle_events(Engine* engine, Viewport* vp) {
  SDL_Event event;
  bool is_dragging = false;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      engine->is_running = false;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      if (event.button.button == SDL_BUTTON_RIGHT) {
        viewport_pan(vp, event.motion.x, event.motion.y);
      }
      if (event.button.button == SDL_BUTTON_LEFT) {
        is_dragging = true;
      }
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
      is_dragging = false;
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
      if (is_dragging) {
        viewport_pan(vp, event.motion.xrel, event.motion.yrel);
      }
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
      float factor = event.wheel.y;
      viewport_zoom_at(vp, (int)event.wheel.mouse_x, (int)event.wheel.mouse_y, factor);
    }
    if (event.type == SDL_EVENT_KEY_DOWN) {
      switch (event.key.key) {
        case SDLK_ESCAPE:
        case SDLK_Q:
          engine->is_running = false;
          break;
        case SDLK_1:
          engine->palette_index = 0;
          break;
        case SDLK_2:
          engine->palette_index = 1;
          break;
        case SDLK_3:
          engine->palette_index = 2;
          break;
        case SDLK_4:
          engine->palette_index = 3;
          break;
        case SDLK_5:
          engine->palette_index = 4;
          break;
        case SDLK_C:
          engine->palette_index = (engine->palette_index + 1) % 5;
          break;
        case SDLK_R:
          viewport_init(vp, 0.0, 0.0, 350, engine->width, engine->height);
          break;
        case SDLK_QUESTION:
          printf("showing help");
        default:
          break;
      }
    }
  }

  return 0;
}


void engine_parallel_for(Engine* engine, int start, int end, ParallelForFunc func, void* user_data) {
  thread_pool_parallel_for(&engine->thread_pool, start, end, func, user_data);
}

int engine_cleanup(Engine* engine) {
  thread_pool_destroy(&engine->thread_pool);
  SDL_DestroyTexture(engine->texture);
  SDL_DestroyRenderer(engine->renderer);
  SDL_DestroyWindow(engine->window);
  SDL_Quit();

  return 0;
}