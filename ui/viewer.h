#ifndef VIEWER_H
#define VIEWER_H

#include <stdbool.h>
#include <stdint.h>

// CSV data structure
typedef struct {
  float *time;
  float *x;     // Cart position
  float *theta; // Pole angle
  float *vx;    // Cart velocity
  float *omega; // Pole angular velocity
  int count;
  int max_count;
} SimulationData;

// Load CSV file
bool viewer_load_csv(const char *filename, SimulationData *data);

// Free loaded data
void viewer_free_data(SimulationData *data);

// Run the visualization
void viewer_run(const char *csv_file);

#endif // VIEWER_H
