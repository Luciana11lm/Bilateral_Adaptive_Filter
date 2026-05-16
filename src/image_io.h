#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <stdint.h>

// Structure used to store grayscale image data
typedef struct {
  int width;
  int height;
  uint8_t *data;
} Image;

// Function used to load a PGM image from file
Image *load_pgm(const char *filename);

// Function used to save a PGM image to file
int save_pgm(const char *filename, const Image *img);

// Function used to free allocated image memory
void free_image(Image *img);

#endif