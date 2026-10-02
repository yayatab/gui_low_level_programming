#pragma once
#include <stdbool.h>

typedef struct {
  double center_x, center_y;
  double zoom; // pixels per unit of math space
  int screen_width, screen_height;
  bool dirty;
} Viewport;

typedef struct {
  double x;
  double y;
} Vec2d;

#pragma region Initialization & Sizing
/**
 * Initializes the viewport.
 * @param vp viewport output
 * @param center_x center of the vp
 * @param center_y center op the vp
 * @param zoom zoom factor
 * @param screen_w the width of the screen
 * @param screen_h the height of the screen
 */
void viewport_init(Viewport *vp, double center_x, double center_y, double zoom, int screen_w, int screen_h);

/**
 * Updates internal pixel dimensions when the SDL window resizes, ensuring aspect ratio corrections remain intact.
 * @param vp viewport to opearate on
 * @param new_w the new x coordiinate
 * @param new_h the new y coordinate
 */
void viewport_resize(Viewport *vp, int new_w, int new_h);

void viewport_recalculate(Viewport *vp);

#pragma endregion

#pragma region Coordinate Transformations

/**
 * Converts a pixel coordinate $(px, py)$ on screen into a mathematical coordinate $(x, y)$
 * @param vp viewport
 * @param out the vector output
 * @param px new x pixel
 * @param py new y pixek
 */
void viewport_screen_to_math(const Viewport *vp, Vec2d *out, int px, int py);

/**
 * Converts a mathematical point $(x, y)$ back into integer pixel coordinates
 * @param vp viewport
 * @param math_x x
 * @param math_y y
 * @param out_px the vp px
 * @param out_py the vp py
 */
void viewport_math_to_screen(const Viewport *vp, double math_x, double math_y, double *out_px, double *out_py);
#pragma endregion

#pragma region Navigation and control

/**
 * Translates the viewport center using raw mouse/keyboard movement deltas
 *
 * Converts screen pixel deltas into mathematical space deltas before modifying center_x and center_y.
 *
 * @param vp Viewport to operate on
 * @param delta_px how mych to pan on the x-axis
 * @param delta_py how much to pan on the y-axis
 */
void viewport_pan(Viewport *vp, double delta_px, double delta_py);

/**
 * Zooms in or out while anchoring the mathematical point under the cursor, keeping the mouse position stationary in math space.
 *
 * Calculates the math coordinate under the mouse prior to zooming, applies the zoom factor, and re-adjusts center_x/center_y so that the anchor point remains under the same screen pixel.
 *
 * @param vp Viewport to operate on
 * @param px
 * @param py
 * @param factor
 */
void viewport_zoom_at(Viewport *vp, int px, int py, double factor);

void viewport_set_center(Viewport* vp, double center_x, double center_y);

#pragma endregion

#pragma region Domain Inspection (Bounding Box)

/**
 * Returns the current bounding box of the visible math space
 *
 * Evaluates top-left $(0,0)$ and bottom-right $(\text{width}, \text{height})$ in math space. Useful for clipping algorithms or thread-chunk generation in your math engine.
 *
 * @param vp
 * @param min_x
 * @param max_x
 * @param min_y
 * @param max_y
 */
void viewport_get_bounds(const Viewport *vp, double *min_x, double *max_x, double *min_y, double *max_y);

#pragma endregion
