#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitStack.h"

const char *FILEPATH_1 = "assets/slike1/";
const char *FILEPATH_2 = "assets/slike2/";
const char *FILEPATH_3 = "assets/slike3/";

bitStack *bmp_comp_binary(uint8_t **image, const int width, const int height);

uint8_t **read_bmp_image(const char *filename, int *width, int *height,
                         int *offset, int *bit_depth, uint8_t **image) {
  char *full_path = (char *)malloc(256 * sizeof(char *));
  // make full string path
  snprintf(full_path, (strlen(FILEPATH_1) + strlen(filename) + 1), "%s%s",
           FILEPATH_1, filename);

  printf("full path made: %s\n", full_path);

  FILE *file = fopen(full_path, "rb");
  if (!file) {
    perror("Can't open file!");
    exit(1);
  }

  // 10th byte has offset info - where pixel info starts
  fseek(file, 10, SEEK_SET);
  fread(offset, sizeof(int), 1, file);

  // 18th byte has width info
  fseek(file, 18, SEEK_SET);
  fread(width, sizeof(int), 1, file);

  // 22nd byte has height info
  fseek(file, 22, SEEK_SET);
  fread(height, sizeof(int), 1, file);

  // 28th byte has bit depth - how many bits per pixel
  fseek(file, 28, SEEK_SET);
  fread(bit_depth, sizeof(int), 1, file);

  // input all pixel data into image array
  // first set 2d array
  image = (uint8_t **)malloc((*height) * (int)sizeof(uint8_t *));
  for (int i = 0; i < (*height); i++) {
    image[i] = (uint8_t *)malloc((*width) * (int)sizeof(uint8_t));
    if (image[i] == NULL) {
      perror("Can't allocate memory for image array!");
      exit(1);
    }
  }

  fseek(file, *offset, SEEK_SET);
  for (int i = 0; i < *height; i++) {
    if (fread(image[i], sizeof(uint8_t), *width, file) != (size_t)(*width)) {
      perror("Error reading image data!");
      exit(1);
    }
  }

  fclose(file);
  return image;
}

int main() {
  const char *filepath = "Baboon.bmp";
  int width, height, offset, bit_depth;
  uint8_t **image =
      (uint8_t **)malloc(sizeof(uint8_t *) * (int)sizeof(uint8_t *));
  image = read_bmp_image(filepath, &width, &height, &offset, &bit_depth, image);

  if (width != 0 && height != 0 && offset != 0 && bit_depth != 0) {
    printf("Širina slike: %d px\n", width);
    printf("Višina slike: %d px\n", height);
    printf("Offset: %d\n", offset);
    printf("Bit depth: %d\n", bit_depth);
  } else {
    printf("Napaka pri branju dimenzij slike.\n");
  }

  // print out first 10 pixels
  for (int i = 0; i < 10; i++) {
    printf("%d ", image[0][i]);
  }

  bitStack *bmp_binary = bmp_comp_binary(image, width, height);

  // write bits to file
  FILE *file = fopen("compressed_image.bin", "wb");

  if (!file) {
    perror("Can't open file for writing!");
    exit(1);
  }
  // write bits to file
  fwrite(bmp_binary->bits, sizeof(long), bmp_binary->size * sizeof(long), file);
  // write number of bits to file
  fwrite(&bmp_binary->size, sizeof(int), 1, file);
  // write number of pixels to file
  fwrite(&width, sizeof(int), 1, file);
  // finish writing
  fclose(file);

  return 0;
}
