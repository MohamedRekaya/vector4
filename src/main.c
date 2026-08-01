#include "physics_engine.h"
#include <stdio.h>

int main() {
  params_t p = {.mass_cart = 0.5f,
                .mass_pole = 0.1f,
                .length = 0.5f,
                .gravity = 9.81f,
                .damping = 0.01f};

  state_t s;
  physics_reset(&s);

  control_t u = {.force = 0.0f};

  float dt = 0.01f;

  printf("time,x,theta,vx,omega\n");

  for (int i = 0; i < 500; i++) {
    float t = i * dt;

    // Push the cart at certain times to test response
    if (i > 100 && i < 110)
      u.force = 0.5f;
    else if (i > 200 && i < 210)
      u.force = -0.5f;
    else
      u.force = 0.0f;

    physics_step(&s, &u, dt, &p);

    printf("%.3f, %.4f, %.4f, %.4f, %.4f\n", t, s.x, s.theta, s.vx, s.omega);
  }
  return 0;
}
