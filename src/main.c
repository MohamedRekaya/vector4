#include "physics_engine.h"
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================
// TEST CASES - Uncomment ONE at a time
// ============================================================

// Case 1: FREE FALL - No force, pole evolves naturally
// #define TEST_FREE_FALL

// Case 2: IMPULSE - Single push at t=1s
// #define TEST_IMPULSE

// Case 3: STEP RESPONSE - Constant force after t=2s
// #define TEST_STEP_RESPONSE

// Case 4: SINE WAVE - Oscillating force
#define TEST_SINE_WAVE

// Case 5: HANGING PENDULUM - Natural oscillation
// #define TEST_HANGING_PENDULUM

// Case 6: MULTI-IMPULSE - Multiple pushes
// #define TEST_MULTI_IMPULSE

// Case 7: CHIRP - Sweeping frequency
// #define TEST_CHIRP

// ============================================================

int main(void) {
  // ============================================================
  // PHYSICAL PARAMETERS
  // ============================================================

  params_t p = {.mass_cart = 0.5f,
                .mass_pole = 0.1f,
                .length = 0.5f,
                .gravity = 9.81f,
                .damping = 0.0f};

  // ============================================================
  // INITIAL STATE
  // ============================================================

  state_t s;
  physics_reset(&s);

  // Hanging pendulum initial condition
#ifdef TEST_HANGING_PENDULUM

  // If theta = 0 is upright, theta = pi is hanging down.
  // Start 0.64 rad (~37 degrees) away from the downward position.
  s.theta = 2.5f;

#endif

  // ============================================================
  // CONTROL INPUT
  // ============================================================

  control_t u = {.force = 0.0f};

  // ============================================================
  // SIMULATION PARAMETERS
  // ============================================================

  const float dt = 0.005f;      // 5 ms
  const int total_steps = 4000; // 20 seconds

  printf("time,x,theta,vx,omega,force\n");

  // ============================================================
  // SIMULATION LOOP
  // ============================================================

  for (int i = 0; i < total_steps; i++) {

    float t = i * dt;

    // ========================================================
    // DETERMINE CONTROL INPUT
    // ========================================================

#ifdef TEST_FREE_FALL

    // No external force.
    u.force = 0.0f;

#elif defined(TEST_IMPULSE)

    // 5 N force applied for 50 ms at t = 1 s.
    if (t >= 1.0f && t < 1.05f) {
      u.force = 5.0f;
    } else {
      u.force = 0.0f;
    }

#elif defined(TEST_STEP_RESPONSE)

    // Constant 2 N force beginning at t = 2 s.
    if (t >= 2.0f) {
      u.force = 2.0f;
    } else {
      u.force = 0.0f;
    }

#elif defined(TEST_SINE_WAVE)

    // 2 N amplitude, 0.5 Hz sine wave.
    float frequency = 0.5f;

    u.force = 2.0f * sinf(2.0f * M_PI * frequency * t);

#elif defined(TEST_HANGING_PENDULUM)

    // No external force.
    u.force = 0.0f;

#elif defined(TEST_MULTI_IMPULSE)

    // Positive impulses at 1, 3, 5 seconds.
    if ((t >= 1.0f && t < 1.05f) || (t >= 3.0f && t < 3.05f) ||
        (t >= 5.0f && t < 5.05f)) {

      u.force = 5.0f;
    }

    // Negative impulses at 2 and 4 seconds.
    else if ((t >= 2.0f && t < 2.05f) || (t >= 4.0f && t < 4.05f)) {

      u.force = -5.0f;
    }

    else {
      u.force = 0.0f;
    }

#elif defined(TEST_CHIRP)

    // Linear chirp:
    // frequency sweeps from 0.2 Hz to 1.0 Hz
    // over 20 seconds.

    const float f0 = 0.2f;
    const float f1 = 1.0f;
    const float T = 20.0f;

    const float k = (f1 - f0) / T;

    float phase = 2.0f * M_PI * (f0 * t + 0.5f * k * t * t);

    u.force = 3.0f * sinf(phase);

#else

    // Default: no force.
    u.force = 0.0f;

#endif

    // ========================================================
    // PHYSICS INTEGRATION
    // ========================================================

    physics_step(&s, &u, dt, &p);

    // ========================================================
    // NO RESET
    // ========================================================
    //
    // The state is never artificially modified.
    //
    // The simulation evolves continuously from its initial
    // condition until the end of the simulation.
    //
    // ========================================================

    // ========================================================
    // INSTABILITY CHECK
    // ========================================================
    //
    // Don't clamp the state.
    // If the simulation becomes numerically unstable,
    // stop and report it.
    //

    if (!isfinite(s.x) || !isfinite(s.theta) || !isfinite(s.vx) ||
        !isfinite(s.omega)) {

      fprintf(stderr, "Simulation became invalid at t = %.3f s\n", t);

      break;
    }

    // Optional sanity check.
    // This does NOT modify the state.
    if (fabsf(s.x) > 100.0f) {

      fprintf(stderr, "Simulation diverged at t = %.3f s, x = %.3f\n", t, s.x);

      break;
    }

    // ========================================================
    // OUTPUT
    // ========================================================

    printf("%.3f,%.6f,%.6f,%.6f,%.6f,%.3f\n", t, s.x, s.theta, s.vx, s.omega,
           u.force);
  }

  return 0;
}
