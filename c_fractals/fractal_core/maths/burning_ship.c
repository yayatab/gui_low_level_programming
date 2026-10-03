#include "burning_ship.h"

#include <math.h>

#define DEFAULT_ESCAPE_RADIUS_SQ 16

void burning_ship_init_interface(FractalInterface* out) {
  if (!out) return;
  out->name = "burning_ship";
  out->calculate_escape = &burning_ship_calculate_escape;
  out->fractal_type = BURNING_SHIP;
  out->destroy = &burning_ship_destroy;
  out->get_default_viewport = &burning_ship_get_default_viewport;
}

double burning_ship_calculate_escape(double math_x, double math_y, double max_iter, const void* _) {
  if (max_iter == 0) {
    return 0.0;
  }
  double z_r = 0.0, z_i = 0.0, z_r2 = 0.0, z_i2 = 0.0;

  double counter = 0;

  // if (check_inside_set(math_x, math_y)) { // TODO like in mandelbrot
  //   return max_iter;
  // }

  while (counter < max_iter && z_r2 + z_i2 <= DEFAULT_ESCAPE_RADIUS_SQ) {
    z_i = 2. * fabs(z_r * z_i) - math_y;
    z_r = z_r2 - z_i2 + math_x;

    z_i2 = z_i * z_i;
    z_r2 = z_r * z_r;
    ++counter;
  }

  if (counter < max_iter) {
    return counter + 1.0 - log2(0.5 * log(z_i2 + z_r2));
  }
  return max_iter;
}

void burning_ship_get_default_viewport(double* out_center_x, double* out_center_y, double* out_zoom) {
  *out_center_x = -0.45;
  *out_center_y = 0.5;
  *out_zoom = 220.;
}

void burning_ship_destroy(void* _) {}