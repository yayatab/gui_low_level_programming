#include "mandelbrot_set.h"

#include <math.h>

#define DEFAULT_ESCAPE_RADIUS_SQ 16.0
#define LN2 0.693147180559945309417
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static const float64x2_t V_TWO = {2.0, 2.0};
static const float64x2_t V_ZERO = {0.0, 0.0};
static const float64x2_t V_ESCAPE_SQ = {DEFAULT_ESCAPE_RADIUS_SQ, DEFAULT_ESCAPE_RADIUS_SQ};

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

double mandelbrot_calculate_escape(double math_x, double math_y, double max_iter, const void* _) {
  if (max_iter == 0) {
    return 0.0;
  }

  double z_r = 0.0, z_i = 0.0;
  double z_r2 = 0.0, z_i2 = 0.0;

  double counter = 0;

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
    return counter + 1.0 - log2(0.5 * log(z_i2 + z_r2));
  }
  return max_iter;
}

float64x2_t mandelbrot_calculate_escape_x2(double math_x, double dx, double math_y, size_t max_iter, const void* _) {
  if (max_iter == 0) { return V_ZERO; }
  float64x2_t v_cr = {math_x, math_x + dx};
  float64x2_t v_ci = vdupq_n_f64(math_y);
  uint64x2_t v_iter = vdupq_n_u64(0);

  float64x2_t v_zr = vdupq_n_f64(0.0);
  float64x2_t v_zi = vdupq_n_f64(0.0);
  float64x2_t v_zr2 = vdupq_n_f64(0.0);
  float64x2_t v_zi2 = vdupq_n_f64(0.0);
  float64x2_t mag_sq = vaddq_f64(v_zr2, v_zi2);

  uint64x2_t active_mask = vcleq_f64(mag_sq, V_ESCAPE_SQ);

  for (size_t counter = 0; counter < max_iter; ++counter) {
    v_iter = vsubq_u64(v_iter, active_mask);
    if ((vgetq_lane_u64(active_mask, 0) | vgetq_lane_u64(active_mask, 1)) == 0) {
      break;
    }
    v_zi = vaddq_f64(vmulq_f64(V_TWO, vmulq_f64(v_zr, v_zi)), v_ci);
    v_zr = vaddq_f64(v_cr, vsubq_f64(v_zr2, v_zi2));
    v_zi2 = vmulq_f64(v_zi, v_zi);
    v_zr2 = vmulq_f64(v_zr, v_zr);
    mag_sq = vaddq_f64(v_zr2, v_zi2);
    active_mask = vcleq_f64(mag_sq, V_ESCAPE_SQ);
  }

  return vcvtq_f64_u64(v_iter);
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