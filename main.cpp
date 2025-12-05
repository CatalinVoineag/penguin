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
global_variable void *pixels;
global_variable int textureWidth;

void SDL_ResizeTexture(SDL_Renderer *renderer, SDL_Window *window) {
  if (texture) {
    SDL_DestroyTexture(texture);
  }

  if (pixels) {
    free(pixels);
  }

  int width;
  int height;
  SDL_GetWindowSize(window, &width, &height);

  SDL_Texture* texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    width,
    height
  );
  textureWidth = width;

  void *pixels = malloc(width * height * 4);
}

void SDL_UpdateWindow(SDL_Renderer *renderer) {
  if (SDL_UpdateTexture(texture, 0, pixels, textureWidth * 4)) {
    SDL_LogError(
      SDL_LOG_CATEGORY_ERROR,
      "Could not update texture: %s\n",
      SDL_GetError()
    );
  } 

  SDL_RenderTexture(renderer, texture, 0, 0);
  SDL_SetRenderDrawColor(renderer,  255, 0, 250, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
  SDL_Window *window;
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("An SDL3 window", 640, 480, SDL_WINDOW_EXTERNAL);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, 0);
  SDL_SetRenderDrawColor(renderer,  255, 190, 10, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  if (window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  running = true;
  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_EVENT_WINDOW_EXPOSED:
          printf("EXPOSED\n");
          SDL_UpdateWindow(renderer);
          break;
        case SDL_EVENT_QUIT:
          running = false;
          break;
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
          printf("FOCUS!\n");
          SDL_SetRenderDrawColor(renderer, 111, 200, 100, 0);
          SDL_RenderClear(renderer);
          SDL_RenderPresent(renderer);
          break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
          SDL_SetRenderDrawColor(renderer,  255, 190, 10, 0);
          SDL_RenderClear(renderer);
          SDL_RenderPresent(renderer);
          printf("LOST FOCUS!\n");
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
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
