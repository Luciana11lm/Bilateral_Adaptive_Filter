#ifndef BILATERAL_H
#define BILATERAL_H

#include <stdint.h>

#include "image_io.h"

#define KERNEL_RADIUS 1
#define KERNEL_SIZE 3
#define INTENSITY_LEVELS 256
#define WEIGHT_SCALE 1024

typedef struct {
  uint16_t spatial_lut[KERNEL_SIZE][KERNEL_SIZE];
  uint16_t range_lut[INTENSITY_LEVELS];
} BilateralLut;

// Function used to initialize bilateral filter LUTs
void bilateral_init_lut(BilateralLut *lut,
                        float sigma_spatial,
                        float sigma_range);

// Function used to apply a 3x3 bilateral filter into an already allocated image
int bilateral_filter_3x3_inplace_output(const Image *input,
                                        Image *output,
                                        const BilateralLut *lut);

#endif