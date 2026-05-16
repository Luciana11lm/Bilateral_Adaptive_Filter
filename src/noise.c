#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "noise.h"

// Function used to generate gaussian random values
// using Box-Muller transform
static float gaussian_random(float mean, float stddev)
{
  static int has_spare = 0;
  static float spare;

  if (has_spare) {
    has_spare = 0;
    return mean + stddev * spare;
  }

  has_spare = 1;

  float u, v, s;

  do {
    u = (rand() / ((float) RAND_MAX)) * 2.0f - 1.0f;
    v = (rand() / ((float) RAND_MAX)) * 2.0f - 1.0f;
    s = u * u + v * v;
  } while (s >= 1.0f || s == 0.0f);

  s = sqrtf(-2.0f * logf(s) / s);

  spare = v * s;

  return mean + stddev * (u * s);
}

// Function used to add gaussian noise to image
void add_gaussian_noise(Image *img, float sigma)
{
  int size = img->width * img->height;

  // Initialize random seed
  //srand(time(NULL));
  // Add reproductible noise
  srand(0);

  for (int i = 0; i < size; i++) {
    float noise = gaussian_random(0.0f, sigma);
    int noisy_pixel = (int)(img->data[i] + noise);

    // Clamp values to valid grayscale interval
    if (noisy_pixel < 0) {
      noisy_pixel = 0;
    }

    if (noisy_pixel > 255) {
      noisy_pixel = 255;
    }

    img->data[i] = (uint8_t)noisy_pixel;
  }
}