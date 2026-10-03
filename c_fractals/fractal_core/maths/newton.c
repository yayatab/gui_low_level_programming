#include "newton.h"
#include <math.h>

#define TOLERANCE_SQ 1e-6
#define SQRT3_OVER_2 0.86602540378443864676

static const double ROOTS_RE[3] = {1.0, -0.5, -0.5};
static const double ROOTS_IM[3] = {0.0, SQRT3_OVER_2, -SQRT3_OVER_2};

void newton_fractal_init_interface(FractalInterface* out) {
  if (!out) return;
  out->name = "newton_fractal";
  out->calculate_escape = &newton_fractal_calculate_escape;
  out->fractal_type = FRACTAL_NEWTON;
  out->destroy = &newton_fractal_destroy;
  out->get_default_viewport = &newton_fractal_get_default_viewport;
}

double newton_fractal_calculate_escape(double math_x, double math_y, double max_iter, const void* custom_params) {
  (void)custom_params;
  if (max_iter <= 0.0) {
    return 0.0;
  }

  double z_r = math_x;
  double z_i = math_y;
  double counter = 0.0;

  while (counter < max_iter) {
    // Check convergence to any of the 3 roots of z^3 - 1 = 0
    for (int k = 0; k < 3; ++k) {
      double dx = z_r - ROOTS_RE[k];
      double dy = z_i - ROOTS_IM[k];
      if (dx * dx + dy * dy < TOLERANCE_SQ) {
        // Map root index (0, 1, 2) to spaced palette bands, shaded by iteration count
        return (double)k * (50.0 / 3.0) + counter;
      }
    }

    // Newton step: z_{n+1} = 2/3 * z_n + 1 / (3 * z_n^2)
    // 1 / z^2 = (z_r^2 - z_i^2 - 2 * z_r * z_i * i) / (z_r^2 + z_i^2)^2
    double r2 = z_r * z_r + z_i * z_i;
    double d = r2 * r2;
    if (d < 1e-12) {
      // Singularity at origin (f'(0) = 0)
      return max_iter;
    }

    double next_r = (2.0 / 3.0) * z_r + (z_r * z_r - z_i * z_i) / (3.0 * d);
    double next_i = (2.0 / 3.0) * z_i - (2.0 * z_r * z_i) / (3.0 * d);

    z_r = next_r;
    z_i = next_i;
    counter += 1.0;
  }

  return max_iter;
}

void newton_fractal_get_default_viewport(double* out_center_x, double* out_center_y, double* out_zoom) {
  if (out_center_x) *out_center_x = 0.0;
  if (out_center_y) *out_center_y = 0.0;
  if (out_zoom) *out_zoom = 250.0;
}

void newton_fractal_destroy(void* custom_data) {
  (void)custom_data;
}