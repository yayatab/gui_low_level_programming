#pragma once

#include <stdint.h>

#define PALETTE_LUT_SIZE 1024
#define PALETTE_LUT_MASK (PALETTE_LUT_SIZE - 1)

typedef uint32_t colour_t;

typedef struct {
  uint32_t palette[PALETTE_LUT_SIZE];
} PaletteLUT;


typedef struct {
  float a[3];
  float b[3];
  float c[3];
  float d[3];
} CosinePalette;

extern const CosinePalette Rainbow;
extern const CosinePalette Snowman;
extern const CosinePalette FireGold;
extern const CosinePalette NeonElectric;
extern const CosinePalette CoolOcean;

colour_t create_colour(int r, int g, int b, int a);

uint32_t palette_sample_cosine(const CosinePalette* palette, float t);

void palette_lut_init(PaletteLUT* lut, const CosinePalette* palette);

/**
 * Maps t -> index in LUT
 * @param lut
 * @param t
 * @return
 */
uint32_t palette_lut_sample(const PaletteLUT* lut, float t);