#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float x, y;
} Vec2f;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  int width;
  int height;
  float scale;  // Pixels per meter
  Vec2f offset; // Screen center offset
} GraphicsContext;

// Initialize graphics
bool graphics_init(GraphicsContext *ctx, const char *title, int width,
                   int height);

// Cleanup
void graphics_cleanup(GraphicsContext *ctx);

// Clear screen with color (0xRRGGBB)
void graphics_clear(GraphicsContext *ctx, uint32_t color);

// Present frame
void graphics_present(GraphicsContext *ctx);

// Drawing functions
void graphics_draw_line(GraphicsContext *ctx, int x0, int y0, int x1, int y1,
                        uint32_t color);
void graphics_draw_rect(GraphicsContext *ctx, int x, int y, int w, int h,
                        uint32_t color);
void graphics_draw_fill_rect(GraphicsContext *ctx, int x, int y, int w, int h,
                             uint32_t color);
void graphics_draw_circle(GraphicsContext *ctx, int x, int y, int radius,
                          uint32_t color);
void graphics_draw_fill_circle(GraphicsContext *ctx, int x, int y, int radius,
                               uint32_t color);

// Convert world coordinates to screen
Vec2f graphics_world_to_screen(GraphicsContext *ctx, float wx, float wy);

#endif // GRAPHICS_H
