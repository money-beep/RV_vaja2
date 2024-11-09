#include <stdint.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void predict_value_inverse(long *p_values, int width, int height,
                           uint8_t **image) {
  image[0][0] = (uint8_t)p_values[0];
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (y == 0) {
        image[x][y] = image[x - 1][y] - (uint8_t)p_values[y * width + x];
      } else if (x == 0) {
        image[x][y] = (uint8_t)(image[x][y - 1] + p_values[y * width + x]);
      } else {
        if (image[x - 1][y - 1] >= MAX(image[x - 1][y], image[x][y - 1])) {
          p_values[y * width + x] = MIN(image[x - 1][y], image[x][y - 1]);
        } else if (image[x - 1][y - 1] <=
                   MIN(image[x - 1][y], image[x][y - 1])) {
          p_values[y * width + x] = MAX(image[x - 1][y], image[x][y - 1]);
        } else {
          p_values[y * width + x] =
              image[x - 1][y] + image[x][y - 1] - image[x - 1][y - 1];
        }
      }
    }
  }
}
