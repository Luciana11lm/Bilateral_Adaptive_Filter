#ifndef BILATERAL_H
#define BILATERAL_H

#include "image_io.h"

// Function used to apply a 3x3 bilateral filter and allocate the output image
Image *bilateral_filter_3x3(const Image *input,
                            float sigma_spatial,
                            float sigma_range);

// Function used to apply a 3x3 bilateral filter into an already allocated image
int bilateral_filter_3x3_inplace_output(const Image *input,
                                        Image *output,
                                        float sigma_spatial,
                                        float sigma_range);

#endif