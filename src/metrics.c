#include <stdio.h>
#include <math.h>

#include "metrics.h"

// Function used to compute Mean Squared Error
float compute_mse(const Image *reference, const Image *test)
{
  if (!reference || !test) {
    return -1.0f;
  }

  if (reference->width != test->width || reference->height != test->height) {
    return -1.0f;
  }

  int size = reference->width * reference->height;

  double sum = 0.0;

  for (int i = 0; i < size; i++) {
    int difference = (int)reference->data[i] - (int)test->data[i];
    sum += (double)(difference * difference);
  }

  return (float)(sum / size);
}

// Function used to compute Peak Signal-to-Noise Ratio
float compute_psnr(float mse)
{
  if (mse <= 0.0f) {
    return 999.0f;
  }

  return 10.0f * log10f((255.0f * 255.0f) / mse);
}