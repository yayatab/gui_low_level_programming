#include "julia.h"

#include <math.h>
#include <complex.h>

#define DEFAULT_ESCAPE_RADIUS_SQ 4.0

static const float64x2_t V_TWO = {2.0, 2.0};
static const float64x2_t V_ESCAPE_SQ = {DEFAULT_ESCAPE_RADIUS_SQ, DEFAULT_ESCAPE_RADIUS_SQ};

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
  out->calc_escape = &julia_calculate_escape;
  out->calc_escape_2x = julia_calculate_escape_x2;
  out->destroy = &julia_destroy;
  out->get_default_viewport = &julia_get_default_viewport;
}


float64x2_t julia_calculate_escape_x2(double math_x, double dx, double math_y, size_t max_iter,
                                      const void* custom_params) {
  JuliaConfig julia_params = JULIA_CLASSIC_SPIRAL;
  if (custom_params != NULL) {
    julia_params = *(JuliaConfig*)custom_params;
  }

  float64x2_t v_jci = vdupq_n_f64(julia_params.c_i);
  float64x2_t v_jcr = vdupq_n_f64(julia_params.c_r);

  float64x2_t v_zr = {math_x, math_x + dx};
  float64x2_t v_zi = vdupq_n_f64(math_y);

  uint64x2_t v_iter = vdupq_n_u64(0);

  float64x2_t v_zr2 = vmulq_f64(v_zr, v_zr);
  float64x2_t v_zi2 = vmulq_f64(v_zi, v_zi);
  float64x2_t mag_sq = vaddq_f64(v_zr2, v_zi2);


  uint64x2_t active_mask = vcleq_f64(mag_sq, V_ESCAPE_SQ);

  for (size_t counter = 0; counter < max_iter; ++counter) {
    v_iter = vsubq_u64(v_iter, active_mask);
    if ((vgetq_lane_u64(active_mask, 0) | vgetq_lane_u64(active_mask, 1)) == 0) {
      break;
    }
    float64x2_t v_tmp_mul = vmulq_f64(v_zr, v_zi);
    v_zi = vaddq_f64(vmulq_f64(V_TWO, v_tmp_mul), v_jci);
    v_zr = vaddq_f64(vsubq_f64(v_zr2, v_zi2), v_jcr);

    v_zr2 = vmulq_f64(v_zr, v_zr);
    v_zi2 = vmulq_f64(v_zi, v_zi);
    mag_sq = vaddq_f64(v_zr2, v_zi2);
    active_mask = vcleq_f64(mag_sq, V_ESCAPE_SQ);
  }

  return vcvtq_f64_u64(v_iter);

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
    z_r = z_r2 - z_i2 /* + math_x */ + julia_params.c_r;

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