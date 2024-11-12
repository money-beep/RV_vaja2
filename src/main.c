#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitStack.h"

const char *FILEPATH_1 = "assets/slike1/";
const char *FILEPATH_2 = "assets/slike2/";
const char *FILEPATH_3 = "assets/slike3/";

bitStack *bmp_comp_binary(uint8_t **image, const int width, const int height);

uint8_t **bmp_decomp_binary(bitStack *bmp_compressed);

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
  /*const char *filepath = "Baboon.bmp";
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
  */
  uint8_t **image = (uint8_t **)malloc(4 * sizeof(uint8_t *));

  for (int i = 0; i < 4; i++) {
    image[i] = (uint8_t *)malloc(5 * sizeof(uint8_t));
  }

  image[0][0] = 23;
  image[0][1] = 21;
  image[0][2] = 21;
  image[0][3] = 23;
  image[0][4] = 23;
  image[1][0] = 24;
  image[1][1] = 22;
  image[1][2] = 22;
  image[1][3] = 20;
  image[1][4] = 24;
  image[2][0] = 23;
  image[2][1] = 22;
  image[2][2] = 22;
  image[2][3] = 19;
  image[2][4] = 23;
  image[3][0] = 26;
  image[3][1] = 25;
  image[3][2] = 21;
  image[3][3] = 19;
  image[3][4] = 22;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 5; j++) {
      printf("%d ", image[i][j]);
    }
    printf("\n");
  }
  bitStack *bmp_binary = bmp_comp_binary(image, 4, 5);

  uint8_t **image_decompressed = (uint8_t **)malloc(4 * sizeof(uint8_t *));

  for (int i = 0; i < 4; i++) {
    image_decompressed[i] = (uint8_t *)malloc(5 * sizeof(uint8_t));
  }
  printf("Decompression started...\n");
  image_decompressed = bmp_decomp_binary(bmp_binary);
  printf("Decompression finished.\n");

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 5; j++) {
      printf("%d ", image_decompressed[i][j]);
    }
  }

  for (int i = 0; i < 4; i++) {
    free(image[i]);
  }
  free(image);
  return 0;
}
