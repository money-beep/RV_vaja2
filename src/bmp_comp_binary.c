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
void set_header(bitStack *bmp_binary, const int height, const int min,
                const long max, const int num_pix) {
  printf("Setting header...\n");

  encode(bmp_binary, 32, (unsigned int)height);
  encode(bmp_binary, 16, (unsigned int)min);
  printf("set first element: %d\n", min);
  encode(bmp_binary, 32, (unsigned long)max);
  printf("set last element: %ld\n", max);
  encode(bmp_binary, 32, (unsigned int)num_pix);

  printf("Header set\n");
}

bitStack *bmp_comp_binary(uint8_t **image, const int width, const int height) {
  int num_pix = width * height;
  printf("Number of pixels: %d, Width: %d, Height: %d\n", num_pix, width,
         height);
  long *p_values = (long *)malloc(num_pix * sizeof(long));

  // pointer array for proccesing of predicted values
  predict_value(p_values, image, width, height);

  // process predicted values - weave
  long *c_weave = (long *)malloc(num_pix * sizeof(long));
  c_weave[0] = p_values[0];

  for (int i = 1; i < num_pix; i++) {
    if (p_values[i] >= 0)
      c_weave[i] = (p_values[i] * 2);
    else
      c_weave[i] = (abs((int)p_values[i]) * 2 - 1);
  }

  // accumulated values
  long *accumulated_values = (long *)malloc(num_pix * sizeof(long));
  accumulated_values[0] = c_weave[0];
  for (int i = 1; i < num_pix; i++) {
    accumulated_values[i] = accumulated_values[i - 1] + c_weave[i];
  }

  bitStack *bmp_binary = (bitStack *)malloc(sizeof(bitStack));
  bmp_binary->size = 2;
  bmp_binary->bits = (long *)malloc(sizeof(long) * bmp_binary->size);
  bmp_binary->index = 0;
  bmp_binary->top = 0;

  // set header of bmp compressed file
  set_header(bmp_binary, height, p_values[0], accumulated_values[num_pix - 1],
             num_pix);
  compress(bmp_binary, accumulated_values, 0, num_pix - 1);

  return bmp_binary;
}
