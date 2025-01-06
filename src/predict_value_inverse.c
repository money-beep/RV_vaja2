#include <stdint.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void predict_value_inverse(long *p_values, int width, int height,
                           uint8_t **image) {
  image[0][0] = p_values[0];

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = y * width + x;

      if (x == 0 && y == 0) {
        continue;
      } else if (y == 0 && x > 0) {
        image[y][x] = image[y][x - 1] - p_values[idx];
      } else if (x == 0 && y > 0) {
        image[y][x] = image[y - 1][x] - p_values[idx];
      } else if (x > 0 && y > 0) {
        uint8_t a = image[y - 1][x];
        uint8_t b = image[y][x - 1];
        uint8_t c = image[y - 1][x - 1];

        if (c >= MAX(a, b)) {
          image[y][x] = MIN(a, b) - p_values[idx];
        } else if (c <= MIN(a, b)) {
          image[y][x] = MAX(a, b) - p_values[idx];
        } else {
          image[y][x] = a + b - c - p_values[idx];
        }
      }
    }
  }
}
