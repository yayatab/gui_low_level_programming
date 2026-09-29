#include "mandelbrot_set.h"

#include <math.h>

#define DEFAULT_ESCAPE_RADIUS_SQ 16.0
#define LN2 0.693147180559945309417
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static bool check_inside_set(double math_x, double math_y) {
  double x0 = math_x - .25;
  double y2 = math_y * math_y;
  double q = x0 * x0 + y2;
  if (q * (q + x0) < y2 * .25) {
    return true;
  }
  double x1 = math_x + 1.0;
  return 16.0 * (x1 * x1 + y2) <= 1.0;
}

double mandelbrot_calculate_escape(double math_x, double math_y, double max_iter, const void* custom_params) {
  if (max_iter == 0) {
    return 0.0;
  }

  double z_r = 0.0, z_i = 0.0;
  double z_r2 = 0.0, z_i2 = 0.0;

  float counter = 0;

  if (check_inside_set(math_x, math_y)) {
    return max_iter;
  }

  while (counter < max_iter && z_r2 + z_i2 <= DEFAULT_ESCAPE_RADIUS_SQ) {
    z_i = 2. * z_r * z_i + math_y;
    z_r = z_r2 - z_i2 + math_x;

    z_i2 = z_i * z_i;
    z_r2 = z_r * z_r;
    ++counter;
  }

  if (counter < max_iter) {
    return counter + 1.0f - log2f(0.5f * logf((float)(z_i2 + z_r2)));
  }
  return max_iter;
}

static void mandelbrot_destroy(void* custom_data) { (void)custom_data; }

void mandelbrot_get_default_viewport(double* out_center_x, double* out_center_y, double* out_zoom) {
  if (out_center_x) *out_center_x = -0.5;
  if (out_center_y) *out_center_y = 0.0;
  if (out_zoom) *out_zoom = 300.0;
}

void mandelbrot_init_interface(FractalInterface* out) {
  if (!out) return;
  out->name = "mandelbrot_set";
  out->fractal_type = FRACTAL_MANDELBROT;
  out->calculate_escape = &mandelbrot_calculate_escape;
  out->destroy = &mandelbrot_destroy;
  out->get_default_viewport = &mandelbrot_get_default_viewport;
}