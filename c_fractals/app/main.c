#include <SDL3/SDL.h>
#include <stdint.h>
#include <unistd.h>

#include "engine.h"
#include "viewport.h"
#include "mandelbrot_set.h"
#include "palette.h"
#include "renderer_cpu.h"


#define WIN_HEIGHT 720
#define WIN_WIDTH 1280

typedef struct {
  Engine* engine;
  Viewport* vp;
  FractalInterface* mandelbrot;
  uint32_t frameCounter;
} FractalRenderContext;

static void render_fractal_row(int y, void* data) {
  const FractalRenderContext* ctx = (FractalRenderContext*)data;
  Vec2d out;
  for (long x = 0; x < ctx->engine->width; x++) {
    viewport_screen_to_math(ctx->vp, &out, x, y);
    float iterations = ctx->mandelbrot->calculate_escape(out.x, out.y, FRACTAL_MAX_ITERATIONS, NULL);
    const int index = y * ctx->engine->width + x;
    if (iterations >= FRACTAL_MAX_ITERATIONS) {
      ctx->engine->pixel_buffer[index] = create_colour(0, 0, 0, 255);
    } else {
      float t = iterations * 0.02f + ctx->frameCounter * 0.005f;
      ctx->engine->pixel_buffer[index] = palette_sample_cosine(&Rainbow, t);
    }
  }
}

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
  int thread_count = 1;
  int opt;

  while ((opt = getopt(argc, argv, "t:")) != -1) {
    switch (opt) {
      case 't':
        thread_count = strtol(optarg, NULL, 10);
        thread_count = MAX_THREAD_COUNT(thread_count);
        break;
      default:
        fprintf(stderr, "Usage: %s [-v] [-o output_file] [inputs...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  int error = engine_init_mt(&engine, WIN_HEIGHT, WIN_WIDTH, thread_count);

  if (error) {
    printf("Error initializing engine\n");
    return error;
  }

  uint32_t frameCounter = 0;

  Viewport vp;

  viewport_init(&vp, -0.5, 0.0, 350.0, engine.width, engine.height);

  FractalInterface mandelbrot;

  mandelbrot_init_interface(&mandelbrot);

  while (engine.is_running) {
    engine_handle_events(&engine);
    frameCounter++;

    FractalRenderContext ctx = {
        .engine = &engine,
        .vp = &vp,
        .mandelbrot = &mandelbrot,
        .frameCounter = frameCounter
    };

    engine_parallel_for(&engine, 0, engine.height, render_fractal_row, &ctx);

    if (engine_update(&engine)) {
      printf("Engine update failed\n");
      break;
    }
  }

  engine_cleanup(&engine);

  return 0;
}