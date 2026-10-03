#pragma once

#include "fractal.h"

void newton_fractal_init_interface(FractalInterface *out);

double newton_fractal_calculate_escape(double math_x, double math_y, double max_iter, const void *custom_params);

void newton_fractal_get_default_viewport(double *out_center_x, double *out_center_y, double *out_zoom);

void newton_fractal_destroy(void* custom_data);