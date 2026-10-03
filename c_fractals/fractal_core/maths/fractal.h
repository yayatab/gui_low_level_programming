#pragma once
#include <stddef.h>

#define IMPLEMENTED_FRACTALS_NUM 4

static const size_t FRACTAL_MAX_ITERATIONS = 1500;


typedef enum {
  FRACTAL_MANDELBROT,
  FRACTAL_JULIA,
  BURNING_SHIP,
  FRACTAL_NEWTON,
  SIERPINSKI_TRIANGLE, //todo implement
  KOCH_SNOWFLAKE, // todo implement
  BARNSLEY_FAN,  // todo implement
} FRACTAL_TYPES;

typedef struct {
  const char* name;
  FRACTAL_TYPES fractal_type;
  /**
  * Takes a mathematical coordinate (x,y) and returns the iteration result.
   */
  double (*calculate_escape)(double math_x, double math_y, double max_iter, const void *custom_params);
  void (*get_default_viewport)(double *out_center_x, double *out_center_y, double *out_zoom);

  void (*destroy)(void* custom_data);
} FractalInterface;

typedef struct {
  const FractalInterface* vtable;
  size_t max_iterations;
  double escape_radius_sq; // pre-squared to avoid extra calculation to sqrt
  void *custom_data;
} FractalInstance;


double escape_fractal(FractalInterface* config, double x, double y);


