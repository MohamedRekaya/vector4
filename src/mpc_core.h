#ifndef MPC_CORE_H
#define MPC_CORE_H

#include "physics_engine.h"

// ============================================================
// PD
// ============================================================

float pd_control(state_t *s, float target_theta);

// ============================================================
// LQR
// ============================================================

typedef struct {
  float K[4];
} lqr_t;

void lqr_init(lqr_t *lqr, params_t *p);

float lqr_control(lqr_t *lqr, state_t *s, float target_x, float target_theta);

// ============================================================
// MPC
// ============================================================

float compute_cost(state_t *s, float target_x, float target_theta);

float rollout(state_t start, control_t *seq, int horizon, float dt, params_t *p,
              float target_x, float target_theta);

void mpc_optimize(state_t *current, control_t *seq, int horizon, int iterations,
                  float dt, params_t *p, float target_x, float target_theta);

#endif
