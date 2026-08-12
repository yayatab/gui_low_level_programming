#include "engine.h"

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"

#include <stdlib.h>

int engine_init(Engine* engine, const int height, const int width) {
  engine->width = width;
  engine->height = height;

  engine->is_running = false;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Unable to initialize SDL3: %s", SDL_GetError());
    return 1;
  }

  if (!SDL_CreateWindowAndRenderer("SDL3 Pure C Fractal Platform", width, height, 0, &engine->window, &engine->renderer)) {
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

  engine->is_running = true;

  return 0;
}

int engine_update(const Engine* engine){

  if (!engine->is_running) {
    return 1;
  }

  SDL_UpdateTexture(engine->texture, NULL, engine->pixel_buffer, engine->width * sizeof(uint32_t));

  SDL_RenderClear(engine->renderer);
  SDL_RenderTexture(engine->renderer, engine->texture, NULL, NULL);
  SDL_RenderPresent(engine->renderer);
  return 0;
}

int engine_handle_events(Engine* engine){
  SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        engine->is_running = false;
      }
    }

  return 0;
}



int engine_cleanup(Engine* engine) {
  SDL_DestroyTexture(engine->texture);
  SDL_DestroyRenderer(engine->renderer);
  SDL_DestroyWindow(engine->window);
  SDL_Quit();
  return 0;
}