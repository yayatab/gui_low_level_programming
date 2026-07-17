#include <stdio.h>

#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>

#define WIN_HEIGHT 1090
#define WIN_WIDTH 1200

int main(int argc, char* argv[])
{
    printf("Hello world!\n");
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Unable to initialize SDL3: %s", SDL_GetError());
        return 1;
    }
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (!SDL_CreateWindowAndRenderer("SDL3 Pure C Fractal Platform", WIN_WIDTH, WIN_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Could not create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIN_WIDTH, WIN_HEIGHT);

    uint32_t* pixelBuffer = malloc(WIN_WIDTH * WIN_HEIGHT * sizeof(uint32_t));

    if (!pixelBuffer)
    {
        SDL_Log("Failed to allocate heap pixel buffer");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    bool running = true;
    SDL_Event event;
    uint32_t frameCounter = 0;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        frameCounter++;
        // [Fractal math core manipulation loop over pixelBuffer]
        for (int y = 0; y < WIN_HEIGHT; y++) {
            for (int x = 0; x < WIN_WIDTH; x++) {
                const int index = y * WIN_WIDTH + x;
                const uint8_t r = (x + frameCounter) % 255;
                const uint8_t g = (y + frameCounter) % 255;
                const uint8_t b = 150;
                const uint8_t a = 255;
                pixelBuffer[index] = (r << 24) | (g << 16) | (b << 8) | a;
            }
        }

        SDL_UpdateTexture(texture, NULL, pixelBuffer, WIN_WIDTH * sizeof(uint32_t));

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    // 6. Tear down sequence
    free(pixelBuffer);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
