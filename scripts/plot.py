#!/usr/bin/env python3
"""
Vector 4 - Simulation Visualization
Usage: python3 plot.py data/simulation.csv
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 plot.py <csv_file>")
        sys.exit(1)

    csv_file = sys.argv[1]
    df = pd.read_csv(csv_file)

    fig, axes = plt.subplots(3, 1, figsize=(10, 10))

    # Pole angle
    axes[0].plot(df['time'], df['theta'], 'b-', linewidth=1.5)
    axes[0].axhline(0, color='r', linestyle='--', linewidth=1, label='Target (upright)')
    axes[0].set_ylabel('Pole Angle (rad)')
    axes[0].set_title('Vector 4 - Cart-Pole Simulation')
    axes[0].legend()
    axes[0].grid(True)

    # Cart position
    axes[1].plot(df['time'], df['x'], 'g-', linewidth=1.5)
    axes[1].axhline(0, color='r', linestyle='--', linewidth=1)
    axes[1].set_ylabel('Cart Position (m)')
    axes[1].grid(True)

    # Control force
    # We don't log control force yet, but we will in Phase 2
    axes[2].plot(df['time'], df['vx'], 'm-', linewidth=1.5)
    axes[2].set_ylabel('Cart Velocity (m/s)')
    axes[2].set_xlabel('Time (s)')
    axes[2].grid(True)

    plt.tight_layout()
    output_file = csv_file.replace('.csv', '.png')
    plt.savefig(output_file, dpi=150)
    print(f"Plot saved to {output_file}")

if __name__ == "__main__":
    main()
