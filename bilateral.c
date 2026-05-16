#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>

#include "bilateral.h"

// Function used to precompute spatial gaussian weights for a 3x3 kernel
static void compute_spatial_lut(uint16_t spatial_lut[KERNEL_SIZE][KERNEL_SIZE],
                                float sigma_spatial)
{
  float two_sigma_spatial_sq = 2.0f * sigma_spatial * sigma_spatial;

  for (int ky = -KERNEL_RADIUS; ky <= KERNEL_RADIUS; ky++) {
    for (int kx = -KERNEL_RADIUS; kx <= KERNEL_RADIUS; kx++) {

      int distance_sq = kx * kx + ky * ky;

      float weight =
        expf(-(float)distance_sq / two_sigma_spatial_sq);

      spatial_lut[ky + KERNEL_RADIUS][kx + KERNEL_RADIUS] =
        (uint16_t)(weight * WEIGHT_SCALE + 0.5f);
    }
  }
}

// Function used to precompute range gaussian weights for all intensity differences
static void compute_range_lut(uint16_t range_lut[INTENSITY_LEVELS],
                              float sigma_range)
{
  float two_sigma_range_sq = 2.0f * sigma_range * sigma_range;

  for (int difference = 0; difference < INTENSITY_LEVELS; difference++) {

    float weight =
      expf(-((float)(difference * difference)) / two_sigma_range_sq);

    range_lut[difference] =
      (uint16_t)(weight * WEIGHT_SCALE + 0.5f);
  }
}

// Function used to initialize all LUTs required by the bilateral filter
void bilateral_init_lut(BilateralLut *lut,
                        float sigma_spatial,
                        float sigma_range)
{
  compute_spatial_lut(lut->spatial_lut, sigma_spatial);
  compute_range_lut(lut->range_lut, sigma_range);
}

// Function used to copy only image borders from input to output
static void copy_image_borders(const Image *input, Image *output)
{
  int width = input->width;
  int height = input->height;

  const uint8_t * restrict in = input->data;
  uint8_t * restrict out = output->data;

  // Copy first and last row
  for (int x = 0; x < width; x++) {
    out[x] = in[x];
    out[(height - 1) * width + x] =
      in[(height - 1) * width + x];
  }

  // Copy first and last column
  for (int y = 0; y < height; y++) {
    out[y * width] = in[y * width];
    out[y * width + width - 1] =
      in[y * width + width - 1];
  }
}

// Function used to apply a 3x3 bilateral filter into an already allocated image
int bilateral_filter_3x3_inplace_output(const Image *input,
                                        Image *output,
                                        const BilateralLut *lut)
{
  if (!input || !output || !lut) {
    return 0;
  }

  if (input->width != output->width ||
      input->height != output->height) {
    return 0;
  }

  int width = input->width;
  int height = input->height;

  const uint8_t * restrict in = input->data;
  uint8_t * restrict out = output->data;

  copy_image_borders(input, output);

  #pragma omp parallel for schedule(static)
  for (int y = 1; y < height - 1; y++) {
    for (int x = 1; x < width - 1; x++) {

      int center_index = y * width + x;
      int center_pixel = in[center_index];

      uint64_t weighted_sum = 0;
      uint64_t weight_sum = 0;

      for (int ky = -KERNEL_RADIUS; ky <= KERNEL_RADIUS; ky++) {
        for (int kx = -KERNEL_RADIUS; kx <= KERNEL_RADIUS; kx++) {

          int neighbor_index =
            (y + ky) * width + (x + kx);

          int neighbor_pixel = in[neighbor_index];

          int intensity_difference =
            neighbor_pixel - center_pixel;

          if (intensity_difference < 0) {
            intensity_difference = -intensity_difference;
          }

          uint32_t spatial_weight =
            lut->spatial_lut[ky + KERNEL_RADIUS][kx + KERNEL_RADIUS];

          uint32_t range_weight =
            lut->range_lut[intensity_difference];

          uint32_t weight =
            spatial_weight * range_weight;

          weighted_sum +=
            (uint64_t)weight * neighbor_pixel;

          weight_sum += weight;
        }
      }

      int filtered_pixel =
        (int)((weighted_sum + weight_sum / 2) / weight_sum);

      if (filtered_pixel < 0) {
        filtered_pixel = 0;
      }

      if (filtered_pixel > 255) {
        filtered_pixel = 255;
      }

      out[center_index] = (uint8_t)filtered_pixel;
    }
  }

  return 1;
}