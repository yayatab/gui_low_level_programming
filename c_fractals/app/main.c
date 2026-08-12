#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include "engine.h"

#define WIN_HEIGHT 1090
#define WIN_WIDTH 1200

int main(int argc, char* argv[]) {

  Engine engine;

  int error = engine_init(&engine, WIN_HEIGHT, WIN_WIDTH);

  if (error) {
    printf("Error initializing engine\n");
    return error;
  }

  uint32_t frameCounter = 0;

  while (engine.is_running) {
    engine_handle_events(&engine);

    frameCounter++;
    for (int y = 0; y < engine.height; y++) {
      for (int x = 0; x < engine.width; x++) {
        const int index = y * engine.width + x;
        const uint8_t r = (x + frameCounter) % 255;
        const uint8_t g = (y + frameCounter) % 255;
        const uint8_t b = 150;
        const uint8_t a = 255;
        engine.pixel_buffer[index] = (r << 24) | (g << 16) | (b << 8) | a;
      }
    }

    if (engine_update(&engine)) {
      printf("Engine update failed\n");
      break;
    }
  }

  engine_cleanup(&engine);

  return 0;
}
