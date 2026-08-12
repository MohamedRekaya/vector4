#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define SCALE 200.0f

typedef struct {
  float time, x, theta, vx, omega;
} DataPoint;

int main(int argc, char *argv[]) {
  const char *csv_file = (argc > 1) ? argv[1] : "data/simulation.csv";

  printf("Loading: %s\n", csv_file);

  // Load CSV
  FILE *file = fopen(csv_file, "r");
  if (!file) {
    printf("ERROR: Cannot open %s\n", csv_file);
    return 1;
  }

  // Count lines
  char line[256];
  int line_count = 0;
  while (fgets(line, sizeof(line), file))
    line_count++;
  rewind(file);

  // Allocate memory
  DataPoint *data = malloc((line_count - 1) * sizeof(DataPoint));
  if (!data) {
    printf("Memory allocation failed\n");
    fclose(file);
    return 1;
  }

  // Skip header
  if (!fgets(line, sizeof(line), file)) {
    printf("Error reading header\n");
    fclose(file);
    free(data);
    return 1;
  }

  // Read data
  int count = 0;
  while (fgets(line, sizeof(line), file)) {
    if (sscanf(line, "%f,%f,%f,%f,%f", &data[count].time, &data[count].x,
               &data[count].theta, &data[count].vx, &data[count].omega) == 5) {
      count++;
    }
  }
  fclose(file);

  printf("Loaded %d data points\n", count);

  if (count < 2) {
    printf("ERROR: Not enough data points!\n");
    free(data);
    return 1;
  }

  // Print first few points
  printf("\nFirst 5 data points:\n");
  for (int i = 0; i < (count < 5 ? count : 5); i++) {
    printf("  t=%.3f, x=%.4f, theta=%.4f\n", data[i].time, data[i].x,
           data[i].theta);
  }

  // Init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL Error: %s\n", SDL_GetError());
    free(data);
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "Pendulum Viewer (Camera Follow)", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Window Error: %s\n", SDL_GetError());
    free(data);
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    printf("Renderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    free(data);
    SDL_Quit();
    return 1;
  }

  printf("\n=== Controls ===\n");
  printf("SPACE: Play/Pause\n");
  printf("R: Reset to start\n");
  printf("ESC: Quit\n");
  printf("Up/Down: Speed up/slow down\n");
  printf("Left/Right: Step frame\n");
  printf("F: Toggle camera follow\n");
  printf("C: Center camera on cart\n\n");

  // Main loop
  int frame = 0;
  int speed = 1;
  bool playing = true;
  bool running = true;
  bool follow_cart = true;
  SDL_Event event;

  // Camera offset (in world units — the world x-coordinate currently
  // centered on screen). This starts at the cart's initial position so
  // there's no jump on the very first frame.
  float camera_offset_x = data[0].x;

  // How quickly the camera catches up to the cart, expressed as a
  // fraction-per-second rather than a fixed fraction-per-frame. This
  // keeps the follow speed consistent regardless of playback speed.
  const float FOLLOW_RATE = 6.0f; // higher = snappier, lower = laggier
  const float dt = 1.0f / 60.0f;  // matches SDL_Delay(16) below

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = false;
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case SDLK_SPACE:
          playing = !playing;
          printf("%s\n", playing ? "Playing" : "Paused");
          break;
        case SDLK_r:
          frame = 0;
          camera_offset_x = data[0].x;
          printf("Reset to start\n");
          break;
        case SDLK_f:
          follow_cart = !follow_cart;
          printf("%s\n",
                 follow_cart ? "Camera: FOLLOWING cart" : "Camera: FIXED");
          break;
        case SDLK_c:
          if (frame < count) {
            camera_offset_x = data[frame].x;
            printf("Camera centered on cart\n");
          }
          break;
        case SDLK_RIGHT:
          if (frame < count - 1) {
            frame++;
            printf("Frame: %d/%d\n", frame, count);
          }
          break;
        case SDLK_LEFT:
          if (frame > 0) {
            frame--;
            printf("Frame: %d/%d\n", frame, count);
          }
          break;
        case SDLK_UP:
          speed = (speed < 10) ? speed + 1 : speed;
          printf("Speed: %d\n", speed);
          break;
        case SDLK_DOWN:
          speed = (speed > 1) ? speed - 1 : 1;
          printf("Speed: %d\n", speed);
          break;
        }
      }
    }

    if (playing) {
      frame += speed;
      if (frame >= count) {
        frame = count - 1;
        playing = false;
        printf("\nEnd of simulation\n");
      }
    }

    DataPoint *d = &data[frame];

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // ============================================
    // CAMERA FOLLOW LOGIC
    // ============================================

    if (follow_cart) {
      // Exponential smoothing toward the cart's world x-position.
      // Using a time-based rate (not a flat 0.1f-per-frame) keeps
      // this consistent even though `speed` changes how much sim
      // time passes each rendered frame.
      float alpha = 1.0f - expf(-FOLLOW_RATE * dt);
      camera_offset_x += (d->x - camera_offset_x) * alpha;
    } else {
      camera_offset_x = 0.0f;
    }

    // Screen center in world units == camera_offset_x.
    // cx is the screen-space x that corresponds to world x = 0.
    float cx = WINDOW_WIDTH / 2.0f - camera_offset_x * SCALE;
    float cy = WINDOW_HEIGHT / 2.0f;

    // ============================================
    // DRAW EVERYTHING WITH CAMERA OFFSET
    // ============================================
    // IMPORTANT: cx already has camera_offset_x baked in, so every
    // screen position below is just  cx + world_x * SCALE.
    // (The previous version subtracted camera_offset_x a second time
    // here, which fought against the smoothing above and produced
    // the jittery, non-smooth follow.)

    // Draw ground line (extends far)
    lineColor(renderer, 0, cy, WINDOW_WIDTH, cy, 0x444444FF);

    // Draw ground markers (every meter), centered around the camera
    int center_meter = (int)roundf(camera_offset_x);
    for (int meter = center_meter - 10; meter <= center_meter + 10; meter++) {
      float screen_x = cx + meter * SCALE;
      if (screen_x > 0 && screen_x < WINDOW_WIDTH) {
        lineColor(renderer, screen_x, cy - 10, screen_x, cy + 10, 0x666666FF);
        if (meter % 2 == 0) {
          filledCircleColor(renderer, screen_x, cy + 20, 3, 0x888888FF);
        }
      }
    }

    // Draw cart
    float cart_x = cx + d->x * SCALE;
    float cart_y = cy;
    float cart_w = 60;
    float cart_h = 30;

    // Cart body (blue)
    boxColor(renderer, cart_x - cart_w / 2, cart_y - cart_h / 2,
             cart_x + cart_w / 2, cart_y + cart_h / 2, 0x0000FFFF);

    // Cart outline (white)
    rectangleColor(renderer, cart_x - cart_w / 2, cart_y - cart_h / 2,
                   cart_x + cart_w / 2, cart_y + cart_h / 2, 0xFFFFFFFF);

    // Draw pole
    float pole_len = 0.5f * SCALE;
    float end_x = cart_x + pole_len * sinf(d->theta);
    float end_y = cart_y - pole_len * cosf(d->theta);

    // Pole (red, thick)
    thickLineColor(renderer, cart_x, cart_y, end_x, end_y, 5, 0xFF0000FF);

    // Pole tip (yellow circle)
    filledCircleColor(renderer, end_x, end_y, 8, 0xFFFF00FF);

    // Pivot point (white circle)
    filledCircleColor(renderer, cart_x, cart_y, 5, 0xFFFFFFFF);

    // ============================================
    // DRAW TRAIL (show cart path)
    // ============================================

    int trail_start = (frame - 100 > 0) ? frame - 100 : 0;
    for (int i = trail_start; i < frame; i++) {
      float trail_x = cx + data[i].x * SCALE;
      float trail_y = cy;
      float alpha_t = (float)(i - trail_start) / 100.0f;
      int r = (int)(255 * alpha_t);
      int g = (int)(255 * alpha_t);
      int b = (int)(255 * alpha_t);
      filledCircleColor(renderer, trail_x, trail_y, 2,
                        ((Uint32)r << 24) | ((Uint32)g << 16) |
                            ((Uint32)b << 8) | 0xFF);
    }

    // ============================================
    // DRAW INFO OVERLAY
    // ============================================

    char info[256];
    float angle_deg = d->theta * 180.0f / M_PI;
    snprintf(info, sizeof(info),
             "Frame: %d/%d  Time: %.2fs  Angle: %.1f deg  x: %.3fm", frame,
             count, d->time, angle_deg, d->x);
    stringColor(renderer, 10, 10, info, 0xFFFFFFFF);

    // Camera mode indicator
    const char *mode = follow_cart ? "FOLLOWING" : "FIXED";
    stringColor(renderer, 10, 35, mode, follow_cart ? 0x00FF00FF : 0xFFFF00FF);

    // Cart position indicator (shows where cart is relative to camera)
    char pos_info[100];
    snprintf(pos_info, sizeof(pos_info), "Cart: %.2fm  |  Camera: %.2fm", d->x,
             camera_offset_x);
    stringColor(renderer, 10, 60, pos_info, 0x888888FF);

    // Progress bar
    int progress = (frame * 100) / count;
    boxColor(renderer, 10, WINDOW_HEIGHT - 30, 210, WINDOW_HEIGHT - 10,
             0x333333FF);
    boxColor(renderer, 10, WINDOW_HEIGHT - 30, 10 + progress * 2,
             WINDOW_HEIGHT - 10, 0x00FF00FF);

    // Status
    const char *status = playing ? "PLAYING" : "PAUSED";
    stringColor(renderer, WINDOW_WIDTH - 150, 10, status,
                playing ? 0x00FF00FF : 0xFF0000FF);

    // Controls hint
    stringColor(renderer, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 30,
                "F:Follow C:Center", 0x888888FF);

    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(data);

  printf("\nViewer closed.\n");
  return 0;
}
