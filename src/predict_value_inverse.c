#include <stdint.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void predict_value_inverse(long *p_values, int width, int height,
                           uint8_t **image) {
  image[0][0] = p_values[0];

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (x == 0 && y == 0)
        continue;

      else if (x == 0) {
        image[x][y] = image[x][y - 1] - p_values[y * width + x];
      } else if (y == 0) {
        image[x][y] = image[x - 1][y] - p_values[y * width + x];
      } else {
        uint8_t a = image[x - 1][y];
        uint8_t b = image[x][y - 1];
        uint8_t c = image[x - 1][y - 1];

        if (c >= MAX(a, b))
          image[x][y] = MIN(a, b) - p_values[y * width + x];
        else if (c <= MIN(a, b))
          image[x][y] = MAX(a, b) - p_values[y * width + x];
        else
          image[x][y] = a + b - c - p_values[y * width + x];
      }
    }
  }
}
