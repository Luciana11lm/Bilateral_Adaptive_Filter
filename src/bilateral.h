#ifndef BILATERAL_H
#define BILATERAL_H

#include "image_io.h"

// Function used to apply a 3x3 bilateral filter on a grayscale image
Image *bilateral_filter_3x3(const Image *input, float sigma_spatial, float sigma_range);

#endif