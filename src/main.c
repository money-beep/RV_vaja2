#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bitStack.h"

const char *FILEPATH_1 = "assets/slike1/";
const char *FILEPATH_2 = "assets/slike2/";
const char *FILEPATH_3 = "assets/slike3/";

bitStack *bmp_comp_binary(uint8_t **image, const int width, const int height);

uint8_t **bmp_decomp_binary(bitStack *bmp_compressed);

uint8_t **read_bmp_image(const char *filename, int *width, int *height,
                         int *offset, long *orig_size, uint8_t **image) {
  char *full_path = (char *)malloc(256 * sizeof(char *));
  // make full string path
  snprintf(full_path, (strlen(FILEPATH_1) + strlen(filename) + 1), "%s%s",
           FILEPATH_1, filename);

  FILE *orig_file = fopen(full_path, "rb");
  fseek(orig_file, 0, SEEK_END);
  *orig_size = ftell(orig_file);
  fclose(orig_file);

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

void print_statistics(long orig_size, long comp_size, double comp_time,
                      double decomp_time) {
  printf("\n\nStatistics:\n");
  printf("Original size: %ld bytes\n", orig_size);
  printf("Compressed size: %ld bits (%ld bytes)\n", comp_size, comp_size / 8);
  printf("Compression ratio: %.2f%%\n",
         (1 - ((float)comp_size / 8) / orig_size) * 100);
  printf("Compression time: %.2f s\n", comp_time);
  printf("Decompression time: %.2f s\n", decomp_time);
}

int main() {
  const char *filepath = "Barb.bmp";

  long orig_size;
  int width, height, offset;
  uint8_t **image =
      (uint8_t **)malloc(sizeof(uint8_t *) * (int)sizeof(uint8_t *));
  image = read_bmp_image(filepath, &width, &height, &offset, &orig_size, image);

  uint8_t **image_decompressed = (uint8_t **)malloc(width * sizeof(uint8_t *));
  for (int i = 0; i < width; i++) {
    image_decompressed[i] = (uint8_t *)malloc(height * sizeof(uint8_t));
  }

  if (width != 0 && height != 0 && offset != 0) {
    printf("Width: %d px\n", width);
    printf("Height: %d px\n", height);
    printf("Offset: %d\n", offset);
  } else {
    printf("Napaka pri branju dimenzij slike.\n");
  }

  clock_t comp_start, comp_end, decomp_start, decomp_end;

  comp_start = clock();
  bitStack *bmp_binary = bmp_comp_binary(image, width, height);
  comp_end = clock();

  long comp_size = bmp_binary->index * sizeof(long);

  printf("Decompression started...\n");
  decomp_start = clock();
  image_decompressed = bmp_decomp_binary(bmp_binary);
  decomp_end = clock();
  printf("Decompression finished.\n");

  double comp_time = (double)(comp_end - comp_start) / CLOCKS_PER_SEC;
  double decomp_time = (double)(decomp_end - decomp_start) / CLOCKS_PER_SEC;
  print_statistics(orig_size, comp_size, comp_time, decomp_time);

  for (int i = 0; i < width; i++) {
    free(image[i]);
    free(image_decompressed[i]);
  }
  free(image_decompressed);
  free(image);
  return 0;
}
