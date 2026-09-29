#pragma once

#include <stdint.h>

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

uint32_t palette_sample_cosine(const CosinePalette* palette, float t);