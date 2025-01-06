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
           FILEPATH_3, filename);

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

void write_bmp_image_with_grayscale_palette(const char *output_file,
                                            uint8_t **image, int width,
                                            int height) {
  // Calculate row size (padded to 4 bytes) and total image size
  int row_stride =
      (width + 3) & ~3; // Each row is padded to a multiple of 4 bytes
  int header_size = 54; // BMP header size
  uint32_t palette_size = 256 * 4;           // Grayscale palette size
  uint32_t image_size = row_stride * height; // Total pixel data size
  uint32_t file_size = header_size + palette_size + image_size;

  // Open the output file
  FILE *file = fopen(output_file, "wb");
  if (!file) {
    perror("Failed to open output file");
    exit(EXIT_FAILURE);
  }

  // Write BMP file header
  uint8_t file_header[14] = {'B',
                             'M', // Signature
                             file_size & 0xFF,
                             (file_size >> 8) & 0xFF,
                             (file_size >> 16) & 0xFF,
                             (file_size >> 24) & 0xFF, // File size
                             0,
                             0,
                             0,
                             0, // Reserved
                             (header_size + palette_size) &
                                 0xFF, // Pixel data offset
                             ((header_size + palette_size) >> 8) & 0xFF,
                             ((header_size + palette_size) >> 16) & 0xFF,
                             ((header_size + palette_size) >> 24) & 0xFF};
  fwrite(file_header, sizeof(uint8_t), 14, file);

  // Write BMP info header
  uint8_t info_header[40] = {40,
                             0,
                             0,
                             0, // Header size
                             width & 0xFF,
                             (width >> 8) & 0xFF,
                             (width >> 16) & 0xFF,
                             (width >> 24) & 0xFF, // Image width
                             height & 0xFF,
                             (height >> 8) & 0xFF,
                             (height >> 16) & 0xFF,
                             (height >> 24) & 0xFF, // Image height
                             1,
                             0, // Planes
                             8,
                             0, // Bit count (8-bit grayscale)
                             0,
                             0,
                             0,
                             0, // Compression (BI_RGB)
                             image_size & 0xFF,
                             (image_size >> 8) & 0xFF,
                             (image_size >> 16) & 0xFF,
                             (image_size >> 24) & 0xFF, // Image size
                             0,
                             0,
                             0,
                             0, // X pixels per meter
                             0,
                             0,
                             0,
                             0, // Y pixels per meter
                             256,
                             0,
                             0,
                             0, // Colors used
                             256,
                             0,
                             0,
                             0}; // Important colors
  fwrite(info_header, sizeof(uint8_t), 40, file);

  // Write grayscale palette
  for (int i = 0; i < 256; i++) {
    uint8_t color[4] = {i, i, i, 0}; // Blue, Green, Red, Reserved
    fwrite(color, sizeof(uint8_t), 4, file);
  }

  // Write pixel data row by row with padding
  for (int y = 0; y < height; y++) {
    fwrite(image[y], sizeof(uint8_t), width, file); // Write row data
    uint8_t padding[3] = {0, 0, 0};                 // Add padding
    fwrite(padding, sizeof(uint8_t), row_stride - width, file);
  }

  fclose(file);
  printf("Decompressed BMP image saved to %s\n", output_file);
}

int main() {
  const char *filepath = "Rainier.bmp";

  long orig_size;
  int width, height, offset;
  uint8_t **image =
      (uint8_t **)malloc(sizeof(uint8_t *) * (int)sizeof(uint8_t *));
  image = read_bmp_image(filepath, &width, &height, &offset, &orig_size, image);

  uint8_t **image_decompressed = (uint8_t **)malloc(height * sizeof(uint8_t *));
  for (int i = 0; i < height; i++) {
    image_decompressed[i] = (uint8_t *)malloc(width * sizeof(uint8_t));
  }

  if (width != 0 && height != 0 && offset != 0) {
    printf("Width: %d px\n", width);
    printf("Height: %d px\n", height);
    printf("Offset: %d\n", offset);
  } else {
    printf("Error reading image dimensions.\n");
    return 1;
  }

  clock_t comp_start, comp_end, decomp_start, decomp_end;

  printf("Compression started.\n");
  comp_start = clock();
  bitStack *bmp_binary = bmp_comp_binary(image, width, height);
  comp_end = clock();
  printf("Compression finished.\n");

  long comp_size = bmp_binary->index * sizeof(long);

  printf("Decompression started.\n");
  decomp_start = clock();
  image_decompressed = bmp_decomp_binary(bmp_binary);
  decomp_end = clock();
  printf("Decompression finished.\n");

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      printf("%d ", image_decompressed[i][j]);
    }
    printf("\n");
  }
  printf("Original image");
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      printf("%d ", image[i][j]);
    }
    printf("\n");
  }

  double comp_time = (double)(comp_end - comp_start) / CLOCKS_PER_SEC;
  double decomp_time = (double)(decomp_end - decomp_start) / CLOCKS_PER_SEC;
  print_statistics(orig_size, comp_size, comp_time, decomp_time);

  // Write decompressed image to BMP using original header
  write_bmp_image_with_grayscale_palette("decompressed_image.bmp",
                                         image_decompressed, width, height);

  // Free allocated memory
  for (int i = 0; i < height; i++) {
    free(image[i]);
    free(image_decompressed[i]);
  }
  free(image);
  free(image_decompressed);

  return 0;
}
