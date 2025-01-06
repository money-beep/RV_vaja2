#include "bitStack.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void decompress(bitStack *bmp_compressed, int *accumulated_values, int low,
                int high);

uint8_t **predict_value_inverse(long *p_values, int width, int height,
                                uint8_t **image);

void decode_header(bitStack *bmp_compressed, int *height, int *first_element,
                   long *last_element, int *num_pix) {
  // height
  // 16 bits
  unsigned long decoded_value;
  decode(bmp_compressed, 32, &decoded_value);
  *height = (int)decoded_value;
  printf("height decoded: %d\n", *height);

  // first
  // 8 bits
  decode(bmp_compressed, 16, &decoded_value);
  *first_element = (int)decoded_value;
  printf("first element decoded: %d\n", *first_element);

  // last
  // 32 bits
  decode(bmp_compressed, 32, &decoded_value);
  *last_element = (long)decoded_value;
  printf("last element decoded: %ld\n", *last_element);

  // num of pixels
  // 32 bits
  decode(bmp_compressed, 32, &decoded_value);
  *num_pix = (int)decoded_value;
  printf("number of pixels decoded: %d\n", *num_pix);
}

uint8_t **bmp_decomp_binary(bitStack *bmp_compressed) {
  int height, first_element, num_pix;
  long last_element;

  bmp_compressed->index = 0;
  bmp_compressed->top = 0;

  decode_header(bmp_compressed, &height, &first_element, &last_element,
                &num_pix);

  int width = num_pix / height;

  int *accumulated_values = (int *)malloc(num_pix * sizeof(int));
  accumulated_values[0] = first_element;
  accumulated_values[num_pix - 1] = last_element;

  decompress(bmp_compressed, accumulated_values, 0, num_pix - 1);

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
  predict_value_inverse(p_values, width, height, image);
  return image;
}
