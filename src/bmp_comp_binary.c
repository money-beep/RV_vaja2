#include "bitStack.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void predict_value(long *p_values, uint8_t **image, const int width,
                   const int height);
void compress(bitStack *bmp_binary, long *c_weave, int start, int end);
void encode(bitStack *cValues, int needed_bits, long value);

// height - 16 bits
// first element - 8 bits
// last element - 32 bits
// num of pixels - 32 bits
void set_header(bitStack *bmp_binary, const int height, const long min,
                const long max, const int num_pix) {
  printf("Setting header...\n");
  // height
  // 16 bits
  encode(bmp_binary, 16, height);
  printf("Height set!\n");
  // first
  // 8 bits
  encode(bmp_binary, 8, min);
  printf("First set!\n");
  // last
  // 32 bits
  encode(bmp_binary, 32, max);
  printf("Last set!\n");
  // num of pixels
  // 32 bits
  encode(bmp_binary, 32, num_pix);
  printf("Header set!\n");
}

bitStack *bmp_comp_binary(uint8_t **image, const int width, const int height) {
  int num_pix = width * height;
  printf("Number of pixels: %d\n", num_pix);
  long *p_values = (long *)malloc(num_pix * sizeof(long));

  // pointer array for proccesing of predicted values
  predict_value(p_values, image, width, height);

  long *c_weave = (long *)malloc(num_pix * sizeof(long));
  c_weave[0] = p_values[0];

  for (int i = 1; i < num_pix; i++) {
    if (p_values[i] >= 0)
      c_weave[i] = c_weave[i - 1] + (p_values[i] * 2);
    else
      c_weave[i] = c_weave[i - 1] + (abs((int)p_values[i]) * 2 - 1);
  }

  // accumulated values
  printf("Accumulating values...\n");
  long *accumulated_values = (long *)malloc(num_pix * sizeof(long));
  accumulated_values[0] = c_weave[0];
  for (int i = 1; i < num_pix; i++) {
    accumulated_values[i] = accumulated_values[i - 1] + c_weave[i];
  }
  printf("Values accumulated!\n");

  bitStack *bmp_binary = (bitStack *)malloc(sizeof(bitStack));
  bmp_binary->size = 2;
  bmp_binary->bits = (long *)malloc(sizeof(long) * bmp_binary->size);
  bmp_binary->index = 0;
  bmp_binary->top = 0;

  set_header(bmp_binary, height, c_weave[0], c_weave[num_pix - 1], num_pix);
  printf("Starting compression...\n");
  compress(bmp_binary, c_weave, 0, num_pix - 1);

  printf("Compression done!\n");

  return bmp_binary;
}
