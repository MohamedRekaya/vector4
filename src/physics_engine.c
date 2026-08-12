#include "physics_engine.h"
#include <math.h>

void physics_reset(state_t *s) {
  s->x = 0.0f;
  s->theta = 0.1f; // slightly tilted
  s->vx = 0.0f;
  s->omega = 0.0f;
}

void physics_step(state_t *s, control_t *u, float dt, params_t *p) {
  float mc = p->mass_cart;
  float mp = p->mass_pole;
  float L = p->length;
  float g = p->gravity;
  float damp = p->damping;
  float F = u->force;

  float sin_t = sinf(s->theta);
  float cos_t = cosf(s->theta);

  // Cart acceleration (this part is correct)
  float denom = mc + mp * sin_t * sin_t;
  float ax =
      (F - damp * s->vx + mp * sin_t * (L * s->omega * s->omega + g * cos_t)) /
      denom;

  // FIXED: Pole angular acceleration for INVERTED pendulum
  float alpha = (g * sin_t + cos_t * ax) / L; // CORRECT: plus signs

  // Semi-implicit Euler integration
  s->vx += ax * dt;
  s->omega += alpha * dt;
  s->x += s->vx * dt;
  s->theta += s->omega * dt;
}
