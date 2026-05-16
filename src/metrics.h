#ifndef METRICS_H
#define METRICS_H

#include "image_io.h"

// Function used to compute Mean Squared Error
float compute_mse(const Image *reference, const Image *test);

// Function used to compute Peak Signal-to-Noise Ratio
float compute_psnr(float mse);

#endif