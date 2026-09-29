#pragma once
#include "fractal.h"
#include <stdbool.h>

/**
 * Optional configuration passed via custom_params.
 * If custom_params is NULL, default configuration is used:
 *   - escape_radius_sq = 4.0
 *   - smooth = true
 *   - check_cardioid_bulb = true
 */
typedef struct {
  double escape_radius_sq;  // Bailout radius squared. If <= 0, defaults to 4.0.
  bool smooth;              // If true, returns continuous normalized iteration count. Default: true.
  bool check_cardioid_bulb; // If true, fast early exit for main cardioid & period-2 bulb. Default: true.
} MandelbrotConfig;

/**
 * Initialize a FractalInterface struct for the Mandelbrot set.
 *
 * @param out Output interface struct to populate.
 */
void mandelbrot_init_interface(FractalInterface *out);

/**
 * Core escape-time algorithm for the Mandelbrot set.
 * Given mathematical point c = (math_x, math_y), iterates z_{n+1} = z_n^2 + c from z_0 = 0.
 *
 * @param math_x Real component of c.
 * @param math_y Imaginary component of c.
 * @param max_iter Maximum iteration limit.
 * @param custom_params Optional pointer to MandelbrotConfig (or NULL for defaults).
 * @return Iteration result in [0, max_iter]. Points inside the set return max_iter.
 */
double mandelbrot_calculate_escape(double math_x, double math_y, double max_iter, const void *custom_params);

/**
 * Default viewport getter for Mandelbrot set.
 */
void mandelbrot_get_default_viewport(double *out_center_x, double *out_center_y, double *out_zoom);
