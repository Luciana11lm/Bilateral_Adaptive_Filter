#ifndef NOISE_H
#define NOISE_H

#include "image_io.h"

// Function used to add gaussian noise to image
void add_gaussian_noise(Image *img, float sigma);

#endif