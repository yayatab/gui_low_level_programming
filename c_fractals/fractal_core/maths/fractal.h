#pragma once
#include <stdlib.h>

static const size_t FRACTAL_MAX_ITERATIONS = 1000;

typedef enum {
  FRACTAL_MANDELBROT,
  FRACTAL_JULIAN,
  FRACTAL_NEWTON,
  SIERPINSKI_TRIANGLE,
  KOCH_SNOWFLAKE,
  BARNSLEY_FAN,
} FRACTAL_TYPES;

typedef struct {
  const char* name;
  FRACTAL_TYPES fractal_type;
  /**
  * Takes a mathematical coordinate (x,y) and returns the iteration result.
   */
  size_t (*calculate_escape)(double math_x, double math_y, size_t max_iter, const void *custom_params);
  void (*get_default_viewport)(double *out_center_x, double *out_center_y, double *out_zoom);

  void (*destroy)(void* custom_data);
} FractalInterface;

typedef struct {
  const FractalInterface* vtable;
  size_t max_iterations;
  double escape_radius_sq; // pre-squared to avoid extra calculation to sqrt
  void *custom_data;
} FractalInstance;


int escape_fractal(FractalInterface* config, double x, double y);


