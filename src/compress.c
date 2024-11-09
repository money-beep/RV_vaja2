#include "bitStack.h"
#include <math.h>

void encode(bitStack *cValues, int needed_bits, long value);

// interpolative compression - using mid range values
void compress(bitStack *bmp_binary, long *accumulated_values, int low,
              int high) {
  if (high - low > 1) {
    if (accumulated_values[high] != accumulated_values[low]) {
      int mid = (int)floor((double)(low + high) / 2);
      long range = accumulated_values[high] - accumulated_values[low] + 1;
      int needed_bits = (int)ceil(log2(range));
      encode(bmp_binary, needed_bits,
             accumulated_values[mid] - accumulated_values[low]);
      if (low < mid) {
        compress(bmp_binary, accumulated_values, low, mid);
      }
      if (mid < high) {
        compress(bmp_binary, accumulated_values, mid, high);
      }
    }
  }
}
