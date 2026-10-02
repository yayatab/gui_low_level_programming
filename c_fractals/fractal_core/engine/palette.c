#include "palette.h"

#include <math.h>

#define TWO_PI 6.283185307179586f
#define CLAMP(x) ((x) < 0.0f ? 0.0f : ((x) > 1.0f ? 1.0f : (x)))

const CosinePalette FireGold = {
    .a = {0.5f, 0.5f, 0.5f},
    .b = {0.5f, 0.5f, 0.5f},
    .c = {2.0f, 1.0f, 0.0f},
    .d = {0.50f, 0.20f, 0.25f}
};

const CosinePalette Rainbow = {
    .a = {0.5f, 0.5f, 0.5f},
    .b = {0.5f, 0.5f, 0.5f},
    .c = {1.0f, 1.0f, 1.0f},
    .d = {0.00f, 0.33f, 0.67f}
};

const CosinePalette Snowman = {
    .a = {0.85f, 0.85f, 0.90f},
    .b = {0.15f, 0.15f, 0.10f},
    .c = {1.0f, 1.0f, 1.0f},
    .d = {0.00f, 0.10f, 0.20f}
};

const CosinePalette NeonElectric = {
    .a = {0.8f, 0.5f, 0.4f},
    .b = {0.2f, 0.4f, 0.2f},
    .c = {2.0f, 1.0f, 1.0f},
    .d = {0.00f, 0.25f, 0.25f}
};

const CosinePalette CoolOcean = {
    .a = {0.5f, 0.5f, 0.5f},
    .b = {0.5f, 0.5f, 0.5f},
    .c = {1.0f, 1.0f, 0.5f},
    .d = {0.80f, 0.90f, 0.30f}
};


uint32_t palette_sample_cosine(const CosinePalette* pal, float t) {
  int rgba[3] = {0, 0, 0};
  for (int i = 0; i < 3; ++i) {
    float angle = TWO_PI * (pal->c[i] * t + pal->d[i]);
    float val = pal->a[i] + pal->b[i] * cosf(angle);
    rgba[i] = (int)(CLAMP(val) * 255.0f);
  }
  return create_colour(rgba[0], rgba[1], rgba[2], 255);
}

void palette_lut_init(PaletteLUT* lut, const CosinePalette* palette) {
  for (int i = 0; i < PALETTE_LUT_SIZE; ++i) {
    float t = i / (float)PALETTE_LUT_SIZE;
    lut->palette[i] = palette_sample_cosine(palette, t);
  }
}

uint32_t palette_lut_sample(const PaletteLUT* lut, float t) {
  uint32_t scalted_t = (uint32_t)(t * (float)PALETTE_LUT_SIZE);
  return lut->palette[scalted_t & PALETTE_LUT_MASK];
}

colour_t create_colour(int r, int g, int b, int a) {
  return (r << 24) | (g << 16) | (b << 8) | a;
}