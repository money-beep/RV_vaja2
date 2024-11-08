#include "bitStack.h"
#include <math.h>

void encode(bitStack *cValues, int needed_bits, long value);

// interpolative compression - using mid range values
void compress(bitStack *bmp_binary, long *c_weave, int low, int high) {
  if (high - low > 1) {
    if (c_weave[high] != c_weave[low]) {
      int mid = (int)floor((double)(low + high) / 2);
      long range = c_weave[high] - c_weave[low] + 1;
      int needed_bits = (int)ceil(log2(range));
      encode(bmp_binary, needed_bits, c_weave[mid] - c_weave[low]);
      if (low < mid) {
        compress(bmp_binary, c_weave, low, mid);
      }
      if (mid < high) {
        compress(bmp_binary, c_weave, mid, high);
      }
    }
  }
}
