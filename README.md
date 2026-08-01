# Vector 4

**A minimal, real-time, differentiable physics engine in C for embedded robotics.**

---

## Overview

Vector 4 is a lightweight physics and control framework designed for resource-constrained robotic systems.

The goal is to enable small embedded devices to understand, predict, and adapt to the physical world without requiring powerful processors, GPUs, or external computation.

Vector 4 combines:

- Real-time physics simulation
- Model Predictive Control (MPC)
- Differentiable physics models
- Online system identification

The first target problem is the classic cart-pole system: teaching a motorized cart to balance a pole while adapting to changes in mass, friction, and external conditions.

---

## Core Idea

A traditional controller assumes the system model is already known.

Vector 4 uses a different approach:

1. **Simulate** the system using an internal physics model
2. **Predict** future states using Model Predictive Control
3. **Compare** predictions with real-world behavior
4. **Update** model parameters to reduce prediction error
5. **Adapt** continuously as conditions change

In simple terms:

> A small robot can learn how its own body behaves and improve its control without manual tuning.

---

## Problems Addressed

| Challenge | Vector 4 Approach |
|-----------|------------------|
| Sim-to-real gap | Online adjustment of the physics model |
| Changing payloads | Adaptive mass and friction estimation |
| Limited hardware | Designed for microcontrollers |
| No GPU or heavy frameworks | Pure C implementation with minimal dependencies |

---

## Project Status

| Phase | Description | Status |
|------|-------------|--------|
| 1 | Physics Engine (Symplectic Euler) | ✅ Complete |
| 2 | MPC Rollout (N=10 horizon) | ⏳ In Progress |
| 3 | Differentiable Physics (Jacobians/Gradients) | 📝 Planned |
| 4 | Online System Identification | 📝 Planned |
| 5 | STM32 Hardware Port | 📝 Planned |

---

## Quick Start

### Requirements

| Tool | Version | Purpose |
|------|---------|---------|
| GCC | 4.8+ | C compiler |
| Make | 3.8+ | Build system |
| Python | 3.6+ | Visualization (optional) |
| Matplotlib | Latest | Plot generation (optional) |

---

## Build and Run Simulation

```bash
make run
```

---

## Generate Plots

```bash
make plot
```

---

## Clean Build Artifacts

```bash
make clean
```

---

## Project Structure

```
vector4/
├── src/              # Core C source code
├── scripts/          # Python visualization and analysis tools
├── data/             # Simulation output files
├── docs/             # Documentation
├── build/            # Build artifacts
├── Makefile          # Build configuration
└── README.md         # Project documentation
```

---

## Design Goals

Vector 4 is built around a few principles:

- **Small:** Suitable for embedded systems and microcontrollers
- **Fast:** Real-time execution with predictable performance
- **Transparent:** No black-box machine learning dependencies
- **Portable:** Written in standard C
- **Understandable:** Physics and algorithms remain inspectable

## License

MIT License

See `LICENSE` for details.

---

## Author

[Mohamed Rekaya]

---

