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

  if (!engine->texture) {
    SDL_Log("Failed to create SDL texture: %s", SDL_GetError());
    SDL_DestroyRenderer(engine->renderer);
    SDL_DestroyWindow(engine->window);
    SDL_Quit();
    return 1;
  }

  engine->pixel_buffer = malloc(width * height * sizeof(uint32_t));

  if (!engine->pixel_buffer) {
    SDL_Log("Failed to allocate heap pixel buffer");
    SDL_DestroyTexture(engine->texture);
    SDL_DestroyRenderer(engine->renderer);
    SDL_DestroyWindow(engine->window);
    SDL_Quit();
    return 1;
  }
  engine->iteration_buffer = malloc(width * height * sizeof(float));
  if (!engine->iteration_buffer) {
    free(engine->pixel_buffer);
    SDL_Log("Failed to allocate heap iteration buffer");
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
  static bool is_dragging = false;
  const double pan_step = 30.0; // pixel step for keyboard panning

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        engine->is_running = false;
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_RIGHT) {
          viewport_set_center(vp, event.button.x, event.button.y);
        } else if (event.button.button == SDL_BUTTON_LEFT) {
          is_dragging = true;
        }
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          is_dragging = false;
        }
        break;

      case SDL_EVENT_MOUSE_MOTION:
        if (is_dragging) {
          viewport_pan(vp, event.motion.xrel, event.motion.yrel);
        }
        break;

      case SDL_EVENT_MOUSE_WHEEL:
        if (event.wheel.y != 0.0f) {
          const double factor = (event.wheel.y > 0.0f) ? 1.15 : (1.0 / 1.15);
          viewport_zoom_at(vp, (int)event.wheel.mouse_x, (int)event.wheel.mouse_y, factor);
        }
        break;

      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
          case SDLK_ESCAPE:
          case SDLK_Q:
            engine->is_running = false;
            break;

          // Panning: WASD and Arrow keys
          case SDLK_W:
          case SDLK_UP:
            viewport_pan(vp, 0.0, pan_step);
            break;
          case SDLK_S:
          case SDLK_DOWN:
            viewport_pan(vp, 0.0, -pan_step);
            break;
          case SDLK_A:
          case SDLK_LEFT:
            viewport_pan(vp, pan_step, 0.0);
            break;
          case SDLK_D:
          case SDLK_RIGHT:
            viewport_pan(vp, -pan_step, 0.0);
            break;

          // Zooming: + / - keys (centered on screen)
          case SDLK_PLUS:
          case SDLK_EQUALS:
          case SDLK_KP_PLUS:
            viewport_zoom_at(vp, engine->width / 2, engine->height / 2, 1.15);
            break;
          case SDLK_MINUS:
          case SDLK_KP_MINUS:
            viewport_zoom_at(vp, engine->width / 2, engine->height / 2, 1.0 / 1.15);
            break;

          // Palette switching
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

          // Reset view
          case SDLK_R:
            viewport_init(vp, -0.5, 0.0, 350.0, engine->width, engine->height);
            break;

          case SDLK_QUESTION:
            printf("Controls:\n"
                "  WASD / Arrows : Pan\n"
                "  + / - / Wheel : Zoom\n"
                "  Left-drag     : Pan with mouse\n"
                "  Right-click   : Recenter at cursor\n"
                "  1-5 / C       : Switch palette\n"
                "  R             : Reset viewport\n"
                "  Q / ESC       : Quit\n");
            break;
          default:
            break;
        }
        break;

      default:
        break;
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
  free(engine->iteration_buffer);
  free(engine->pixel_buffer);
  engine->pixel_buffer = NULL;
  engine->iteration_buffer = NULL;

  return 0;
}