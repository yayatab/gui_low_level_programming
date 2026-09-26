#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include "engine.h"
#include "viewport.h"
#include "mandelbrot_set.h"
#include "renderer_cpu.h"

#define WIN_HEIGHT 720
#define WIN_WIDTH 1280

void rainbow_pattern(Engine* engine, uint32_t frameCounter) {
    for (int y = 0; y < engine->height; y++) {
      for (int x = 0; x < engine->width; x++) {
        const int index = y * engine->width + x;
        const uint8_t r = (x + frameCounter) % 255;
        const uint8_t g = (y + frameCounter) % 255;
        const uint8_t b = 150;
        const uint8_t a = 255;
        engine->pixel_buffer[index] = (r << 24) | (g << 16) | (b << 8) | a;
      }
    }
}

int main(int argc, char* argv[]) {

  Engine engine;

  int error = engine_init(&engine, WIN_HEIGHT, WIN_WIDTH);

  if (error) {
    printf("Error initializing engine\n");
    return error;
  }

  uint32_t frameCounter = 0;

  Viewport vp;

  viewport_init(&vp, -0.5, 0.0, 350.0, engine.width, engine.height);

  FractalInterface mandelbrot;

  mandelbrot_init_interface(&mandelbrot);

  size_t iterations = 0;

  Vec2d out;

  while (engine.is_running) {
    engine_handle_events(&engine);
    frameCounter++;

    for (long y = 0; y < engine.height; y++) {
      for (long x = 0; x < engine.width; x++) {
        viewport_screen_to_math(&vp, &out, x, y);
        iterations = mandelbrot.calculate_escape(out.x, out.y, FRACTAL_MAX_ITERATIONS, NULL);
        uint8_t r = (uint8_t)((iterations * 5 + frameCounter) % 256);
        uint8_t g = (uint8_t)((iterations * 11 + frameCounter) % 256);
        uint8_t b = (uint8_t)((iterations * 23 + frameCounter) % 256);
        uint8_t a = 255 ;
        const int index = y * engine.width + x;
        engine.pixel_buffer[index] = (r << 24) | (g << 16) | (b << 8) | a;
        // todo, change to: renderer_cpu_set_pixel(engine.renderer, x, y,create_colour(r, g, b, a) );
      }
    }


    // rainbow_pattern(&engine ,frameCounter);

    if (engine_update(&engine)) {
      printf("Engine update failed\n");
      break;
    }
  }

  engine_cleanup(&engine);

  return 0;
}
