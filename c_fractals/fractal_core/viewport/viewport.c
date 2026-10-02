#include "viewport.h"

#define ASPECT_CORRECTION_RATIO ((double)vp->screen_width / vp->screen_height)

void viewport_init(Viewport* vp,
                   const double center_x,
                   const double center_y,
                   const double zoom,
                   const int screen_w,
                   const int screen_h) {
  vp->center_x = center_x;
  vp->center_y = center_y;
  vp->zoom = zoom;
  vp->screen_width = screen_w;
  vp->screen_height = screen_h;
  vp->dirty = true;
}

void viewport_resize(Viewport* vp, const int new_w, const int new_h) {
  vp->screen_width = new_w;
  vp->screen_height = new_h;
  vp->dirty = true;
}

void viewport_screen_to_math(const Viewport* vp, Vec2d* out, const int px, const int py) {
  const double aspect_correction_ratio = ASPECT_CORRECTION_RATIO;
  const double delta_px = px - vp->screen_width * 0.5;
  const double delta_py = py - vp->screen_height * 0.5;

  const double delta_x = delta_px / vp->zoom * aspect_correction_ratio;
  const double delta_y = delta_py / vp->zoom;

  out->x = vp->center_x + delta_x;
  out->y = vp->center_y - delta_y;
}

void viewport_math_to_screen(
    const Viewport* vp,
    const double math_x,
    const double math_y,
    double* out_px,
    double* out_py) {
  const double delta_x = math_x - vp->center_x;
  const double delta_y = math_y - vp->center_y;
  const double delta_px = delta_x * vp->zoom / ASPECT_CORRECTION_RATIO;
  const double delta_py = delta_y * vp->zoom;
  *out_px = vp->screen_width * 0.5 + delta_px;
  *out_py = vp->screen_height * 0.5 - delta_py;
}

void viewport_pan(Viewport* vp, const double delta_px, const double delta_py) {
  const double delta_math_px = delta_px * ASPECT_CORRECTION_RATIO / vp->zoom;
  const double delta_math_py = delta_py / vp->zoom;
  vp->center_x -= delta_math_px;
  vp->center_y += delta_math_py;
  vp->dirty = true;
}

void viewport_zoom_at(Viewport* vp, const int px, const int py, const double factor) {
  double aspect_correction_ratio = ASPECT_CORRECTION_RATIO;
  const double math_x = vp->center_x + (px - vp->screen_width * 0.5) / vp->zoom * aspect_correction_ratio;
  const double math_y = vp->center_y - (py - vp->screen_height * 0.5) / vp->zoom;

  vp->zoom *= factor;

  vp->center_x = math_x - (px - vp->screen_width * 0.5) / vp->zoom * aspect_correction_ratio;
  vp->center_y = math_y + (py - vp->screen_height * 0.5) / vp->zoom;
  vp->dirty = true;
}

void viewport_set_center(Viewport *vp, double center_x, double center_y) {
  Vec2d target;
  viewport_screen_to_math(vp, &target, (int)center_x, (int)center_y);
  vp->center_x = target.x;
  vp->center_y = target.y;
  vp->dirty = true;
}

void viewport_get_bounds(const Viewport* vp, double* min_x, double* max_x, double* min_y, double* max_y) {
  double aspect_correction_ratio = ASPECT_CORRECTION_RATIO;
  *min_x = vp->center_x - vp->screen_width * 0.5 * aspect_correction_ratio / vp->zoom;
  *max_x = vp->center_x + vp->screen_width * 0.5 * aspect_correction_ratio / vp->zoom;

  *min_y = vp->center_y - vp->screen_height * 0.5 / vp->zoom;
  *max_y = vp->center_y + vp->screen_height * 0.5 / vp->zoom;
}

void viewport_recalculate(Viewport* vp) {
  vp->dirty = true;
}