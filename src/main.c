#include "mpc_core.h"
#include "physics_engine.h"
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TEST_MPC

int main(void) {
  // ============================================================
  // PHYSICAL PARAMETERS (FIXED DAMPING)
  // ============================================================
  params_t p = {.mass_cart = 0.5f,
                .mass_pole = 0.1f,
                .length = 0.5f,
                .gravity = 9.81f,
                .damping = 0.01f,
                .pivot_damping = 0.05f};

  // ============================================================
  // INITIAL STATE
  // ============================================================
  state_t s;
  physics_reset(&s);

// ============================================================
// MPC PARAMETERS (FIXED HORIZON)
// ============================================================
#define MPC_HORIZON 40
#define MPC_ITERATIONS 15
  control_t mpc_sequence[MPC_HORIZON];
  for (int i = 0; i < MPC_HORIZON; i++) {
    mpc_sequence[i].force = 0.0f;
  }
  // ============================================================
  // CONTROL INPUT
  // ============================================================
  control_t u = {.force = 0.0f};

  // ============================================================
  // SIMULATION PARAMETERS
  // ============================================================
  const float dt = 0.005f;
  const int total_steps = 4000;

  printf("time,x,theta,vx,omega,force\n");

  // ============================================================
  // SIMULATION LOOP
  // ============================================================
  for (int i = 0; i < total_steps; i++) {
    float t = i * dt;
    float disturbance =
        (t >= 2.0f && t < 2.1f) ? 10.0f : 0.0f; // 100ms pulse at t=2s
    // ========================================================
    // MPC OPTIMIZATION
    // ========================================================
    mpc_optimize(&s, mpc_sequence, MPC_HORIZON, MPC_ITERATIONS, dt, &p, 0.0f,
                 0.0f);

    // Apply the first control input
    u.force = mpc_sequence[0].force + disturbance;

    // Receding horizon shift
    for (int j = 0; j < MPC_HORIZON - 1; j++) {
      mpc_sequence[j] = mpc_sequence[j + 1];
    }
    mpc_sequence[MPC_HORIZON - 1].force = 0.0f;

    // ========================================================
    // PHYSICS INTEGRATION
    // ========================================================
    physics_step(&s, &u, dt, &p);

    // ========================================================
    // OUTPUT
    // ========================================================
    printf("%.3f,%.6f,%.6f,%.6f,%.6f,%.3f\n", t, s.x, s.theta, s.vx, s.omega,
           u.force);
  }

  return 0;
}
