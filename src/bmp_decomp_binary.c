#include "bitStack.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void decompress(bitStack *bmp_compressed, int *accumulated_values, int low,
                int high);

uint8_t **predict_value_inverse(long *p_values, int width, int height,
                                uint8_t **image);

int decode_header(bitStack *bmp_compressed, int *height, int *first_element,
                  int *last_element, int *num_pix) {
  // height
  // 16 bits
  printf("decoding height\n");
  *height = decode(bmp_compressed, 16);
  printf("height decoded\n");
  // first
  // 8 bits
  printf("decoding first element\n");
  *first_element = decode(bmp_compressed, 8);
  printf("first element decoded\n");
  // last
  // 32 bits
  printf("decoding last element\n");
  *last_element = decode(bmp_compressed, 32);
  printf("last element decoded\n");
  // num of pixels
  // 32 bits
  printf("decoding number of pixels\n");
  *num_pix = decode(bmp_compressed, 32);
  printf("number of pixels decoded\n");
  return 1;
}

uint8_t **bmp_decomp_binary(bitStack *bmp_compressed) {
  int height, first_element, last_element, num_pix;
  printf("Decoding header...\n");
  int temp = decode_header(bmp_compressed, &height, &first_element,
                           &last_element, &num_pix);
  printf("Header decoded");

  int width = num_pix / height;

  int *accumulated_values = (int *)malloc(num_pix * sizeof(int));
  accumulated_values[0] = first_element;
  accumulated_values[num_pix - 1] = last_element;

  printf("decompressing / getting accumulated values");
  decompress(bmp_compressed, accumulated_values, 0, num_pix - 1);
  printf("decompressed values");

  long *c_weave = (long *)malloc(num_pix * sizeof(long));
  c_weave[0] = accumulated_values[0];
  for (int i = 1; i < num_pix; i++) {
    c_weave[i] = accumulated_values[i] - accumulated_values[i - 1];
  }

  long *p_values = (long *)malloc(num_pix * sizeof(long));
  p_values[0] = c_weave[0];
  for (int i = 1; i < num_pix; i++) {
    if (c_weave[i] % 2 == 0) {
      p_values[i] = c_weave[i] / 2;
    } else {
      p_values[i] = -((c_weave[i] + 1) / 2);
    }
  }

  uint8_t **image = (uint8_t **)malloc(height * (int)sizeof(uint8_t *));
  for (int i = 0; i < height; i++) {
    image[i] = (uint8_t *)malloc(width * (int)sizeof(uint8_t));
    if (image[i] == NULL) {
      perror("Can't allocate memory for image array!");
      exit(1);
    }
  }
  printf("Predicting values...\n");
  predict_value_inverse(p_values, width, height, image);
  printf("Predicted values computed!\nFinished!\n");
  return image;
}
