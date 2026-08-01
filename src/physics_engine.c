#include "physics_engine.h"

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

  float denom = mc + mp * sin_t * sin_t;
  float ax = (F + mp * sin_t * (g * cos_t + L * s->omega * s->omega)) / denom;
  ax -= damp * s->vx;

  float alpha = (-g * sin_t - cos_t * ax) / L;

  s->vx += ax * dt;
  s->omega += alpha * dt;
  s->x += s->vx * dt;
  s->theta += s->omega * dt;
}
