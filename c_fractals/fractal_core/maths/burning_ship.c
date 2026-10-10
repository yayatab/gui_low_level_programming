#include "burning_ship.h"

#include <math.h>

#define DEFAULT_ESCAPE_RADIUS_SQ 16

static const float64x2_t V_TWO = {2.0, 2.0};
static const float64x2_t V_ZERO = {0.0, 0.0};
static const float64x2_t V_ESCAPE_SQ = {DEFAULT_ESCAPE_RADIUS_SQ, DEFAULT_ESCAPE_RADIUS_SQ};


void burning_ship_init_interface(FractalInterface* out) {
  if (!out) return;
  out->name = "burning_ship";
  out->calc_escape = &burning_ship_calculate_escape;
  out->calc_escape_2x = &burning_ship_calculate_escape_x2;
  out->fractal_type = BURNING_SHIP;
  out->destroy = &burning_ship_destroy;
  out->get_default_viewport = &burning_ship_get_default_viewport;
}

float64x2_t burning_ship_calculate_escape_x2(double math_x, double dx, double math_y, size_t max_iter, const void* _) {
  float64x2_t v_cr = {math_x, math_x + dx};
  float64x2_t v_ci = vdupq_n_f64(math_y);
  uint64x2_t v_iter = vdupq_n_u64(0);

  float64x2_t v_zr = vdupq_n_f64(0.0);
  float64x2_t v_zi = vdupq_n_f64(0.0);
  float64x2_t v_zr2 = vmulq_f64(v_zr, v_zr);
  float64x2_t v_zi2 = vmulq_f64(v_zi, v_zi);
  float64x2_t mag_sq = vaddq_f64(v_zr2, v_zi2);

  uint64x2_t active_mask = vcleq_f64(mag_sq, V_ESCAPE_SQ);

  for (size_t counter = 0; counter < max_iter; ++counter) {
    v_iter = vsubq_u64(v_iter, active_mask);
    if ((vgetq_lane_u64(active_mask, 0) | vgetq_lane_u64(active_mask, 1)) == 0) {
      break;
    }
    float64x2_t v_tmp_mul = vabsq_f64(vmulq_f64(v_zr, v_zi));
    v_zi = vsubq_f64(vmulq_f64(V_TWO, v_tmp_mul), v_ci);
    v_zr = vaddq_f64(vsubq_f64(v_zr2, v_zi2), v_cr);

    v_zr2 = vmulq_f64(v_zr, v_zr);
    v_zi2 = vmulq_f64(v_zi, v_zi);
    mag_sq = vaddq_f64(v_zr2, v_zi2);
    active_mask = vcleq_f64(mag_sq, V_ESCAPE_SQ);
  }

  return vcvtq_f64_u64(v_iter);
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