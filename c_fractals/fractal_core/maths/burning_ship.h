#pragma once

#include "fractal.h"

void burning_ship_init_interface(FractalInterface *out);

double burning_ship_calculate_escape(double math_x, double math_y, double max_iter, const void *custom_params);

void burning_ship_get_default_viewport(double *out_center_x, double *out_center_y, double *out_zoom);

void burning_ship_destroy(void* custom_data);