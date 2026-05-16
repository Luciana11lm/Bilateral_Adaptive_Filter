#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>

#include "image_io.h"
#include "noise.h"
#include "bilateral.h"
#include "metrics.h"

// Function used to create a copy of an image
static Image *copy_image(const Image *input)
{
  Image *copy = (Image *)malloc(sizeof(Image));

  if (!copy) {
    return NULL;
  }

  copy->width = input->width;
  copy->height = input->height;

  int size = input->width * input->height;

  copy->data = (uint8_t *)malloc(size);

  if (!copy->data) {
    free(copy);
    return NULL;
  }

  memcpy(copy->data, input->data, size);

  return copy;
}

int main(int argc, char *argv[])
{
  if (argc < 4) {
    printf("Usage: %s input.pgm noisy.pgm output.pgm [number_of_runs]\n", argv[0]);
    return 1;
  }

  int number_of_runs = 1;

  // Read number of benchmark iterations
  if (argc >= 5) {
    number_of_runs = atoi(argv[4]);

    if (number_of_runs <= 0) {
      number_of_runs = 1;
    }
  }

  printf("Number of runs : %d\n", number_of_runs);

  double total_execution_time_ms = 0.0;
  double min_execution_time_ms = DBL_MAX;
  double max_execution_time_ms = 0.0;

  // Load original image
  Image *original = load_pgm(argv[1]);

  if (!original) {
    return 1;
  }

  printf("Image loaded successfully\n");
  printf("Width  : %d\n", original->width);
  printf("Height : %d\n", original->height);

  // Create noisy image copy
  Image *noisy = copy_image(original);

  if (!noisy) {
    free_image(original);
    return 1;
  }

  // Add gaussian noise
  add_gaussian_noise(noisy, 20.0f);

  printf("Gaussian noise added\n");

  // Save noisy image
  save_pgm(argv[2], noisy);

  // Apply bilateral filter
  Image *filtered = NULL;

  for (int run = 0; run < number_of_runs; run++) {
    struct timespec start;
    struct timespec end;

    // Start execution timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    filtered = bilateral_filter_3x3(noisy, 1.0f, 25.0f);

    // Stop execution timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    if (!filtered) {
      free_image(original);
      free_image(noisy);
      return 1;
    }

    double execution_time_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
    total_execution_time_ms += execution_time_ms;

    if (execution_time_ms < min_execution_time_ms) {
      min_execution_time_ms = execution_time_ms;
    }

    if (execution_time_ms > max_execution_time_ms) {
      max_execution_time_ms = execution_time_ms;
    }

    // Free previous filtered image except for last iteration
    if (run != number_of_runs - 1) {
      free_image(filtered);
    }
  }

  double average_execution_time_ms = total_execution_time_ms / number_of_runs;

  // Save filtered image
  save_pgm(argv[3], filtered);

  // Compute image quality metrics
  float mse = compute_mse(original, filtered);
  float psnr = compute_psnr(mse);

  printf("\n===== RESULTS =====\n");
  printf("Average execution time : %.3f ms\n", average_execution_time_ms);
  printf("Minimum execution time : %.3f ms\n", min_execution_time_ms);
  printf("Maximum execution time : %.3f ms\n", max_execution_time_ms);
  printf("MSE                    : %.3f\n", mse);
  printf("PSNR                   : %.3f dB\n", psnr);

  // Release allocated memory
  free_image(original);
  free_image(noisy);
  free_image(filtered);

  return 0;
}