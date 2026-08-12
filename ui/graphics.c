#include "graphics.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>

bool graphics_init(GraphicsContext *ctx, const char *title, int width,
                   int height) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return false;
  }

  ctx->window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_SHOWN);
  if (!ctx->window) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    return false;
  }

  ctx->renderer = SDL_CreateRenderer(
      ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!ctx->renderer) {
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return false;
  }

  ctx->width = width;
  ctx->height = height;
  ctx->scale = 200.0f; // 200 pixels per meter
  ctx->offset.x = width / 2.0f;
  ctx->offset.y = height / 2.0f;

  return true;
}

void graphics_cleanup(GraphicsContext *ctx) {
  if (ctx->renderer)
    SDL_DestroyRenderer(ctx->renderer);
  if (ctx->window)
    SDL_DestroyWindow(ctx->window);
  SDL_Quit();
}

void graphics_clear(GraphicsContext *ctx, uint32_t color) {
  SDL_SetRenderDrawColor(ctx->renderer, (color >> 16) & 0xFF,
                         (color >> 8) & 0xFF, color & 0xFF, 255);
  SDL_RenderClear(ctx->renderer);
}

void graphics_present(GraphicsContext *ctx) {
  SDL_RenderPresent(ctx->renderer);
}

void graphics_draw_line(GraphicsContext *ctx, int x0, int y0, int x1, int y1,
                        uint32_t color) {
  lineColor(ctx->renderer, x0, y0, x1, y1, color);
}

void graphics_draw_rect(GraphicsContext *ctx, int x, int y, int w, int h,
                        uint32_t color) {
  rectangleColor(ctx->renderer, x - w / 2, y - h / 2, x + w / 2, y + h / 2,
                 color);
}

void graphics_draw_fill_rect(GraphicsContext *ctx, int x, int y, int w, int h,
                             uint32_t color) {
  boxColor(ctx->renderer, x - w / 2, y - h / 2, x + w / 2, y + h / 2, color);
}

void graphics_draw_circle(GraphicsContext *ctx, int x, int y, int radius,
                          uint32_t color) {
  circleColor(ctx->renderer, x, y, radius, color);
}

void graphics_draw_fill_circle(GraphicsContext *ctx, int x, int y, int radius,
                               uint32_t color) {
  filledCircleColor(ctx->renderer, x, y, radius, color);
}

Vec2f graphics_world_to_screen(GraphicsContext *ctx, float wx, float wy) {
  Vec2f result;
  result.x = ctx->offset.x + wx * ctx->scale;
  result.y = ctx->offset.y - wy * ctx->scale; // Flip Y for screen coords
  return result;
}
