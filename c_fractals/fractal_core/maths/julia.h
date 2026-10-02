#pragma once
#include "fractal.h"



typedef struct {
  double c_r;
  double c_i;
} JuliaConfig;

extern const JuliaConfig JULIA_CLASSIC_SPIRAL;
extern const JuliaConfig JULIA_CLASSIC_SPIRAL2;
extern const JuliaConfig JULIA_DOUDY_RABBIT;
extern const JuliaConfig JULIA_DOUBLE_RABBIT;
extern const JuliaConfig JULIA_BASILLICA;
extern const JuliaConfig JULIA_JETBRAINS;


void julia_init_interface(FractalInterface *out);

double julia_calculate_escape(double math_x, double math_y, double max_iter, const void *custom_params);

void julia_get_default_viewport(double *out_center_x, double *out_center_y, double *out_zoom);

void julia_destroy(void *custom_data);