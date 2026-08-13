#include "mpc_core.h"
#include "physics_engine.h"
#include <string.h>

/////////////////////////////////////////////////////////
/// PD controller
////////////////////////////////////////////////////////

float pd_control(state_t *s, float target_theta) {
  float Kp = 10.0f;
  float Kd = 2.0f;

  float error = target_theta - s->theta;
  float derivative = -s->omega;

  float force = Kp * error + Kd * derivative;

  if (force > 5.0f)
    force = 5.0f;

  if (force < -5.0f)
    force = -5.0f;

  return force;
}

/////////////////////////////////////////////////////////
/// LQR controller
////////////////////////////////////////////////////////

void lqr_init(lqr_t *lqr, params_t *p) {

  lqr->K[0] = -1.0f;
  lqr->K[1] = 21.22f;
  lqr->K[2] = -1.8f;
  lqr->K[3] = 4.31f;
}

float lqr_control(lqr_t *lqr, state_t *s, float target_x, float target_theta) {

  float error_x = s->x - target_x;

  float error_theta = s->theta - target_theta;

  float force = -(lqr->K[0] * error_x + lqr->K[1] * error_theta +
                  lqr->K[2] * s->vx + lqr->K[3] * s->omega);

  if (force > 10.0f)
    force = 10.0f;

  if (force < -10.0f)
    force = -10.0f;

  return force;
}

// ============================================================
// MPC CONTROLLER
// ============================================================
#define MPC_MAX_HORIZON 40
#define MPC_FORCE_LIMIT 15

float compute_cost(state_t *s, float target_x, float target_theta) {
  float x_error = s->x - target_x;
  float theta_error = s->theta - target_theta;

  float x_cost = 40.0f * x_error * x_error;
  float theta_cost = 500.0f * theta_error * theta_error;
  float vx_cost = 2.0f * s->vx * s->vx;
  float omega_cost = 0.5f * s->omega * s->omega;

  return x_cost + theta_cost + vx_cost + omega_cost;
}

float rollout(state_t start, control_t *seq, int horizon, float dt, params_t *p,
              float target_x, float target_theta) {
  state_t s = start;
  float total_cost = 0.0f;

  for (int i = 0; i < horizon; i++) {
    physics_step(&s, &seq[i], dt, p);
    total_cost += compute_cost(&s, target_x, target_theta);
    total_cost += 0.01f * (seq[i].force) * seq[i].force;
  }

  return total_cost;
}

void mpc_optimize(state_t *current, control_t *seq, int horizon, int iterations,
                  float dt, params_t *p, float target_x, float target_theta) {
  if (horizon > MPC_MAX_HORIZON)
    horizon = MPC_MAX_HORIZON;

  // FIXED: Much faster learning rate
  const float learning_rate = 0.02f; // Was 0.0001
  const float eps = 0.001f;

  control_t temp_plus[MPC_MAX_HORIZON];
  control_t temp_minus[MPC_MAX_HORIZON];

  for (int iter = 0; iter < iterations; iter++) {
    float gradient[MPC_MAX_HORIZON];

    // Calculate gradient for each control
    for (int i = 0; i < horizon; i++) {
      memcpy(temp_plus, seq, horizon * sizeof(control_t));
      temp_plus[i].force += eps;

      memcpy(temp_minus, seq, horizon * sizeof(control_t));
      temp_minus[i].force -= eps;

      float cost_plus =
          rollout(*current, temp_plus, horizon, dt, p, target_x, target_theta);
      float cost_minus =
          rollout(*current, temp_minus, horizon, dt, p, target_x, target_theta);

      gradient[i] = (cost_plus - cost_minus) / (2.0f * eps);
    }

    // Apply gradient update
    for (int i = 0; i < horizon; i++) {
      seq[i].force -= learning_rate * gradient[i];

      if (seq[i].force > MPC_FORCE_LIMIT)
        seq[i].force = MPC_FORCE_LIMIT;
      if (seq[i].force < -MPC_FORCE_LIMIT)
        seq[i].force = -MPC_FORCE_LIMIT;
    }
  }
}
