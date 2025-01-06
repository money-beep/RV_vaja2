#include <stdint.h>
#include <stdio.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void predict_value(long *p_values, uint8_t **image, const int width,
                   const int height) {
  printf("Predicting values...\n");
  p_values[0] = image[0][0];

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = y * width + x;

      if (x == 0 && y == 0) {
        continue;
      } else if (y == 0 && x > 0) {
        p_values[idx] = image[y][x - 1] - image[y][x];
      } else if (x == 0 && y > 0) {
        p_values[idx] = image[y - 1][x] - image[y][x];
      } else if (x > 0 && y > 0) {
        if (image[y - 1][x - 1] >= MAX(image[y - 1][x], image[y][x - 1])) {
          p_values[idx] = MIN(image[y - 1][x], image[y][x - 1]) - image[y][x];
        } else if (image[y - 1][x - 1] <=
                   MIN(image[y - 1][x], image[y][x - 1])) {
          p_values[idx] = MAX(image[y - 1][x], image[y][x - 1]) - image[y][x];
        } else {
          p_values[idx] = image[y - 1][x] + image[y][x - 1] -
                          image[y - 1][x - 1] - image[y][x];
        }
      }
    }
  }
  printf("Predicted values computed!\n");
}
