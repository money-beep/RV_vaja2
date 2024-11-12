#include <stdint.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void predict_value_inverse(long *p_values, int width, int height,
                           uint8_t **image) {
  image[0][0] = p_values[0];
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (x == 0 && y == 0) {
        continue;
      } else if (x == 0) {
        image[0][y] = image[0][y - 1] + p_values[y * width + x];
      } else if (y == 0) {
        image[x][0] = image[x - 1][0] - p_values[y * width + x];
      } else {
        if (image[x - 1][y - 1] >= MAX(image[x - 1][y], image[x][y - 1])) {
          image[x][y] =
              (MIN(image[x - 1][y], image[x][y - 1]) - p_values[y * width + x]);
        } else if (image[x - 1][y - 1] <=
                   MIN(image[x - 1][y], image[x][y - 1])) {
          image[x][y] =
              (MAX(image[x - 1][y], image[x][y - 1]) - p_values[y * width + x]);
        } else {
          image[x][y] = (image[x - 1][y] + image[x][y - 1] -
                         image[x - 1][y - 1] - p_values[y * width + x]);
        }
      }
    }
  }
}
