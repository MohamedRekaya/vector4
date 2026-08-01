#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <math.h>

typedef struct {
  float x;     // cart position (m)
  float theta; // pole angle (rad)
  float vx;    // cart velocity (m/s)
  float omega; // pole angular velocity (rad/s)
} state_t;

typedef struct {
  float force; // force on cart (N)
} control_t;

typedef struct {
  float mass_cart;
  float mass_pole;
  float length;
  float gravity;
  float damping;
} params_t;

void physics_step(state_t *s, control_t *u, float dt, params_t *p);
void physics_reset(state_t *s);

#endif
