#include "bitStack.h"
#include <math.h>
#include <stdint.h>

void decompress(bitStack *bmp_compressed, int *accumulated_values, int low,
                int high) {
  if (high - low > 1) {
    if (accumulated_values[high] == accumulated_values[low]) {
      for (int i = low; i < high; i++) {
        accumulated_values[i] = accumulated_values[low];
      }
    } else {
      int mid = (int)floor((double)(low + high) / 2);
      long range = accumulated_values[high] - accumulated_values[low] + 1;
      int needed_bits = (int)ceil(log2(range));
      unsigned long decoded_value;

      decode(bmp_compressed, needed_bits, &decoded_value);
      accumulated_values[mid] = accumulated_values[low] + decoded_value;

      if (low < mid) {
        decompress(bmp_compressed, accumulated_values, low, mid);
      }
      if (mid < high) {
        decompress(bmp_compressed, accumulated_values, mid, high);
      }
    }
  }
}
