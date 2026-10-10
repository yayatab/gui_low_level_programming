#include "newton.h"
#include <stdbool.h>

#define TOLERANCE_SQ 1e-6
#define SQRT3_OVER_2 0.86602540378443864676

static const double ROOTS_RE[3] = {1.0, -0.5, -0.5};
static const double ROOTS_IM[3] = {0.0, SQRT3_OVER_2, -SQRT3_OVER_2};
static const float64x2_t V_ESCAPE_SQ = {TOLERANCE_SQ, TOLERANCE_SQ};
static const float64x2_t V_SINGULARITY = {1e-12, 1e-12};
static const float64x2_t V_TWO_THIRDS = {2.0 / 3.0, 2.0 / 3.0};
static const float64x2_t V_THREE = {3.0, 3.0};

static const float64x2_t V_ROOTS_RE[3] = {
    {1.0, 1.0},
    {-.5, -.5},
    {-.5, -.5},
};

static const float64x2_t V_ROOTS_IM[3] = {
    {0., 0.},
    {SQRT3_OVER_2, SQRT3_OVER_2},
    {-SQRT3_OVER_2, -SQRT3_OVER_2}
};

static const float64x2_t V_TWO = {2.0, 2.0};

void newton_fractal_init_interface(FractalInterface* out) {
  if (!out) return;
  out->name = "newton_fractal";
  out->calc_escape = &newton_fractal_calculate_escape;
  out->calc_escape_2x = &newton_fractal_calculate_escape_x2;
  out->fractal_type = FRACTAL_NEWTON;
  out->destroy = &newton_fractal_destroy;
  out->get_default_viewport = &newton_fractal_get_default_viewport;
}

float64x2_t newton_fractal_calculate_escape_x2(double math_x, double dx, double math_y, size_t max_iter,
                                               const void* _) {
  float64x2_t v_zr = {math_x, math_x + dx};
  float64x2_t v_zi = vdupq_n_f64(math_y);
  float64x2_t v_zr2 = vmulq_f64(v_zr, v_zr);
  float64x2_t v_zi2 = vmulq_f64(v_zi, v_zi);

  bool done_0 = false, done_1 = false;
  double ret_0 = max_iter, ret_1 = max_iter;

  for (size_t counter = 0; counter < max_iter; ++counter) {
    for (size_t k = 0; k < 3; ++k) {
      float64x2_t v_dx = vsubq_f64(v_zr, V_ROOTS_RE[k]);
      float64x2_t v_dy = vsubq_f64(v_zi, V_ROOTS_IM[k]);
      float64x2_t dist_sq = vaddq_f64(vmulq_f64(v_dx, v_dx), vmulq_f64(v_dy, v_dy));
      uint64x2_t hit = vcltq_f64(dist_sq, V_ESCAPE_SQ);
      if (!done_0 && vgetq_lane_u64(hit, 0)) {
        ret_0 = (double)k * (50.0 / 3.0) + (double)counter;
        done_0 = true;
      }
      if (!done_1 && vgetq_lane_u64(hit, 1)) {
        ret_1 = (double)k * (50.0 / 3.0) + (double)counter;
        done_1 = true;
      }
    }
    if (done_0 && done_1) {
      break;
    }

    float64x2_t r2 = vaddq_f64(v_zr2, v_zi2);
    float64x2_t d = vmulq_f64(V_THREE, vmulq_f64(r2, r2));

    float64x2_t num_r = vsubq_f64(v_zr2, v_zi2);
    float64x2_t num_i = vmulq_f64(V_TWO, vmulq_f64(v_zr, v_zi));

    float64x2_t next_r = vaddq_f64(vmulq_f64(V_TWO_THIRDS, v_zr), vdivq_f64(num_r, d));
    float64x2_t next_i = vsubq_f64(vmulq_f64(V_TWO_THIRDS, v_zi), vdivq_f64(num_i, d));

    v_zr = next_r;
    v_zi = next_i;
    v_zr2 = vmulq_f64(v_zr, v_zr);
    v_zi2 = vmulq_f64(v_zi, v_zi);
  }

  return (float64x2_t){ret_0, ret_1};
}

double newton_fractal_calculate_escape(double math_x, double math_y, double max_iter, const void* _) {
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