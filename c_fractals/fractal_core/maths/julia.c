#include "julia.h"

#include <math.h>

#define DEFAULT_ESCAPE_RADIUS_SQ 4.0

const JuliaConfig JULIA_CLASSIC_SPIRAL = {-0.7, 0.27015};
const JuliaConfig JULIA_CLASSIC_SPIRAL2 = {-0.4, 0.6};
const JuliaConfig JULIA_DOUDY_RABBIT = {-0.123, 0.745};
const JuliaConfig JULIA_DOUBLE_RABBIT = {0.285, 0.01};
const JuliaConfig JULIA_BASILLICA = {1, -0.618033988749};
const JuliaConfig JULIA_JETBRAINS = {-0.3, 0.6};


void julia_init_interface(FractalInterface* out) {
  if (!out) return;
  out->name = "julia_set";
  out->fractal_type = FRACTAL_JULIA;
  out->calculate_escape = &julia_calculate_escape;
  out->destroy = &julia_destroy;
  out->get_default_viewport = &julia_get_default_viewport;
}

double julia_calculate_escape(double math_x, double math_y, double max_iter, const void* custom_params) {
  if (max_iter == 0) {
    return 0.0;
  }
  JuliaConfig julia_params = JULIA_CLASSIC_SPIRAL;
  if (custom_params != NULL) {
    julia_params = *(JuliaConfig*)custom_params;
  }

  double z_r = math_x, z_i = math_y;
  double z_r2 = z_r * z_r, z_i2 = z_i * z_i;

  double counter = 0;

  // if (check_inside_set(math_x, math_y)) { // TODO like in mandelbrot
  //   return max_iter;
  // }

  while (counter < max_iter && z_r2 + z_i2 <= DEFAULT_ESCAPE_RADIUS_SQ) {
    z_i = 2. * z_r * z_i /* + math_y */ + julia_params.c_i;
    z_r = z_r2 - z_i2    /* + math_x */ + julia_params.c_r;

    z_i2 = z_i * z_i;
    z_r2 = z_r * z_r;
    ++counter;
  }

  if (counter < max_iter) {
    return counter + 1.0 - log2f(0.5 * log(z_i2 + z_r2));
  }
  return max_iter;
}

void julia_get_default_viewport(double* out_center_x, double* out_center_y, double* out_zoom) {
  if (out_center_x) *out_center_x = -0.0;
  if (out_center_y) *out_center_y = 0.0;
  if (out_zoom) *out_zoom = 300.0;
}

void julia_destroy(void* custom_data) { (void)custom_data; }