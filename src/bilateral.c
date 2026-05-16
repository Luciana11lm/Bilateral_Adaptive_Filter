#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "bilateral.h"

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

// Function used to apply a 3x3 bilateral filter on a grayscale image
Image *bilateral_filter_3x3(const Image *input, float sigma_spatial, float sigma_range)
{
  Image *output = create_image_like(input);

  if (!output) {
    return NULL;
  }

  int width = input->width;
  int height = input->height;

  float two_sigma_spatial_sq = 2.0f * sigma_spatial * sigma_spatial;
  float two_sigma_range_sq = 2.0f * sigma_range * sigma_range;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {

      int center_index = y * width + x;
      int center_pixel = input->data[center_index];

      float weighted_sum = 0.0f;
      float weight_sum = 0.0f;

      // Iterate through the 3x3 neighborhood
      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {

          int nx = clamp_int(x + kx, 0, width - 1);
          int ny = clamp_int(y + ky, 0, height - 1);

          int neighbor_index = ny * width + nx;
          int neighbor_pixel = input->data[neighbor_index];

          int spatial_distance_sq = kx * kx + ky * ky;
          int intensity_difference = neighbor_pixel - center_pixel;

          // Spatial weight depends on pixel position
          float spatial_weight = expf(
            -(float)spatial_distance_sq / two_sigma_spatial_sq
          );

          // Range weight depends on pixel intensity difference
          float range_weight = expf(
            -((float)(intensity_difference * intensity_difference)) / two_sigma_range_sq
          );

          float weight = spatial_weight * range_weight;

          weighted_sum += weight * neighbor_pixel;
          weight_sum += weight;
        }
      }

      // Normalize filtered value
      int filtered_pixel = (int)(weighted_sum / weight_sum + 0.5f);

      if (filtered_pixel < 0) {
        filtered_pixel = 0;
      }

      if (filtered_pixel > 255) {
        filtered_pixel = 255;
      }

      output->data[center_index] = (uint8_t)filtered_pixel;
    }
  }

  return output;
}