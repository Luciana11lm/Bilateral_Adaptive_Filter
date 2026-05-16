#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "bilateral.h"

#define KERNEL_RADIUS 1
#define KERNEL_SIZE 3
#define INTENSITY_LEVELS 256
#define WEIGHT_SCALE 1024

// Function used to clamp pixel coordinates inside image boundaries
static int clamp_int(int value, int min_value, int max_value)
{
  if (value < min_value) {
    return min_value;
  }

  if (value > max_value) {
    return max_value;
  }

  return value;
}

// Function used to allocate a new image with the same size as the input image
static Image *create_image_like(const Image *input)
{
  Image *output = (Image *)malloc(sizeof(Image));

  if (!output) {
    return NULL;
  }

  output->width = input->width;
  output->height = input->height;

  output->data = (uint8_t *)malloc(output->width * output->height);

  if (!output->data) {
    free(output);
    return NULL;
  }

  return output;
}

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

// Function used to apply an optimized 3x3 bilateral filter on a grayscale image
Image *bilateral_filter_3x3(const Image *input,
                            float sigma_spatial,
                            float sigma_range)
{
  Image *output = create_image_like(input);

  if (!output) {
    return NULL;
  }

  if (!bilateral_filter_3x3_inplace_output(input,
                                           output,
                                           sigma_spatial,
                                           sigma_range)) {
    free_image(output);
    return NULL;
  }

  return output;
}

int bilateral_filter_3x3_inplace_output(const Image *input,
                                        Image *output,
                                        float sigma_spatial,
                                        float sigma_range)
{
  if (!input || !output) {
    return 0;
  }

  if (input->width != output->width ||
      input->height != output->height) {
    return 0;
  }

  int width = input->width;
  int height = input->height;

  uint16_t spatial_lut[KERNEL_SIZE][KERNEL_SIZE];
  uint16_t range_lut[INTENSITY_LEVELS];

  compute_spatial_lut(spatial_lut, sigma_spatial);
  compute_range_lut(range_lut, sigma_range);

  // Copy original image to output image.
  // This keeps border pixels unchanged.
  for (int i = 0; i < width * height; i++) {
    output->data[i] = input->data[i];
  }

  for (int y = 1; y < height - 1; y++) {
    for (int x = 1; x < width - 1; x++) {

      int center_index = y * width + x;
      int center_pixel = input->data[center_index];

      uint64_t weighted_sum = 0;
      uint64_t weight_sum = 0;

      for (int ky = -KERNEL_RADIUS; ky <= KERNEL_RADIUS; ky++) {
        for (int kx = -KERNEL_RADIUS; kx <= KERNEL_RADIUS; kx++) {

          int nx = x + kx;
          int ny = y + ky;

          int neighbor_index = ny * width + nx;
          int neighbor_pixel = input->data[neighbor_index];

          int intensity_difference = neighbor_pixel - center_pixel;

          if (intensity_difference < 0) {
            intensity_difference = -intensity_difference;
          }

          uint32_t spatial_weight = spatial_lut[ky + KERNEL_RADIUS][kx + KERNEL_RADIUS];

          uint32_t range_weight = range_lut[intensity_difference];

          uint32_t weight = spatial_weight * range_weight;

          weighted_sum += (uint64_t)weight * neighbor_pixel;
          weight_sum += weight;
        }
      }

      int filtered_pixel = (int)((weighted_sum + weight_sum / 2) / weight_sum);

      if (filtered_pixel < 0) {
        filtered_pixel = 0;
      }

      if (filtered_pixel > 255) {
        filtered_pixel = 255;
      }

      output->data[center_index] = (uint8_t)filtered_pixel;
    }
  }

  return 1;
}