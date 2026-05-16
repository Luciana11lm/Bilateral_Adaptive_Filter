#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "image_io.h"

// Function used to load a binary PGM (P5) image
Image *load_pgm(const char *filename)
{
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error: Cannot open file %s\n", filename);
    return NULL;
  }

  char format[3];

  // Read image format
  fscanf(fp, "%2s", format);

  // Verify PGM P5 format
  if (strcmp(format, "P5") != 0) {
    printf("Error: Unsupported image format\n");
    fclose(fp);
    return NULL;
  }

  Image *img = (Image *)malloc(sizeof(Image));

  if (!img) {
    fclose(fp);
    return NULL;
  }

  int maxval;

  // Read image width, height and maximum pixel value
  fscanf(fp, "%d %d %d", &img->width, &img->height, &maxval);

  // Skip one byte after header
  fgetc(fp);

  // Allocate memory for image pixels
  img->data = (uint8_t *)malloc(img->width * img->height);

  if (!img->data) {
    free(img);
    fclose(fp);
    return NULL;
  }

  // Read image data
  fread(img->data, 1, img->width * img->height, fp);

  fclose(fp);

  return img;
}

// Function used to save image as binary PGM (P5)
int save_pgm(const char *filename, const Image *img)
{
  FILE *fp = fopen(filename, "wb");

  if (!fp) {
    printf("Error: Cannot create file %s\n", filename);
    return 0;
  }

  // Write PGM header
  fprintf(fp, "P5\n%d %d\n255\n", img->width, img->height);

  // Write image pixels
  fwrite(img->data, 1, img->width * img->height, fp);

  fclose(fp);

  return 1;
}

// Function used to release image memory
void free_image(Image *img)
{
  if (img) {
    free(img->data);
    free(img);
  }
}