#include <SDL3/SDL_init.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <cstdio>
#include <cstdlib>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool running;
global_variable SDL_Texture *texture;
global_variable void *bitmapMemory;
global_variable int bitmapWidth;
global_variable int bitmapHeight;
global_variable int bytesPerPixel = 4;

void renderWierdGradient(int xOffset, int yOffset) {
  int pitch = bitmapWidth * bytesPerPixel;
  uint8_t *row = (uint8_t *)bitmapMemory;

  for (int Y = 0; Y < bitmapHeight; Y++) {
    uint32_t *pixel = (uint32_t*)row;
    for (int X = 0; X < bitmapWidth; X++) {
      uint8_t blue = X + xOffset;
      uint8_t green = Y + yOffset;
      uint8_t red = 0;
      uint8_t alpha = 255; // opacity

      // Construct 0xAARRGGBB -> memory on little-endian becomes [BB][GG][RR][AA]
      *pixel++ = ((alpha << 24)| (red << 16) | (green << 8) | blue); 
    } 
   row += pitch; 
  }
}

void SDL_ResizeTexture(SDL_Renderer *renderer, SDL_Window *window) {
  if (texture) {
    SDL_DestroyTexture(texture);
  }

  if (bitmapMemory) {
    free(bitmapMemory);
  }

  int width;
  int height;
  SDL_GetWindowSize(window, &width, &height);

  texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    width,
    height
  );

  bitmapWidth = width;
  bitmapHeight = height;

  bitmapMemory = malloc(width * height * bytesPerPixel);
}

void SDL_UpdateWindow(SDL_Renderer *renderer) {
  if (SDL_UpdateTexture(texture, 0, bitmapMemory, bitmapWidth * bytesPerPixel) != 1) {
    SDL_LogError(
      SDL_LOG_CATEGORY_ERROR,
      "Could not update texture: %s\n",
      SDL_GetError()
    );
  } 

  SDL_RenderTexture(renderer, texture, 0, 0);
  SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
  SDL_Window *window;
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("An SDL3 window", 640, 480, SDL_WINDOW_EXTERNAL);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, 0);
  SDL_RenderPresent(renderer);

  if (window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  running = true;
  int xOffset = 0;
  int yOffset = 0;
  SDL_ResizeTexture(renderer, window);

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_EVENT_QUIT:
          running = false;
          break;
        case SDL_EVENT_WINDOW_EXPOSED:
          printf("EXPOSED\n");
          SDL_UpdateWindow(renderer);
          break;
        case SDL_EVENT_WINDOW_RESIZED: 
          printf("RESIZED SCREEN!\n");
          SDL_ResizeTexture(renderer, window);
          break;
        default:
          // printf("EVENT TYPE: %u\n", event.type);
          break;
      }
    }
    renderWierdGradient(xOffset,0);
    printf("OUTSIDE \n");
    SDL_UpdateWindow(renderer);
    xOffset++;
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
