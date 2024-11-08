#include <stdint.h>
#include <stdio.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void predict_value(long *p_values, uint8_t **image, const int width,
                   const int height) {
  // first value
  printf("Predicting values...\n");
  p_values[0] = image[0][0];
  printf("First value: %ld\n", p_values[0]);

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (x == 0) {
        p_values[y * width + x] = image[0][y - 1] - image[0][y];
      } else if (y == 0) {
        p_values[y * width + x] = image[x - 1][0] - image[x][0];
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

  printf("Predicted values computed!\n");
}
