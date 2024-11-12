#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitStack.h"

void reallocStack(bitStack *cValues) {
  if (cValues->top == 32) {
    cValues->size++;

    long *newBits =
        (long *)realloc(cValues->bits, sizeof(long) * cValues->size);
    if (newBits == NULL) {
      printf("Error: Memory allocation failed!\n");
      exit(1);
    }
    cValues->bits = newBits;
    // new part of the array is set to 0
    memset(cValues->bits + cValues->size - 1, 0, sizeof(long));
    cValues->index++;
    cValues->top = 0;
  }
}

// encodes the value with num of bits
void encode(bitStack *cValues, int needed_bits, long value) {
  for (int i = needed_bits - 1; i >= 0; i--) {
    ensureSpace(cValues);

    // clear first bit
    cValues->bits[cValues->index] &= ~(1L << cValues->top);

    if ((value >> i) & 1L) {
      cValues->bits[cValues->index] |= (1L << cValues->top);
    }
    cValues->top++;
  }
}

void decode(bitStack *cValues, int needed_bits, long *value) {
  *value = 0;
  for (int i = 0; i < needed_bits; i++) {
    ensureReadSpace(cValues);
    if ((cValues->bits[cValues->index] >> cValues->top) & 1L) {
      *value |= (1L << (needed_bits - 1 - i));
    }
    cValues->top++;
  }
}

void ensureSpace(bitStack *cValues) {
  // if bit goes over range
  if (cValues->top >= 32) {
    // call realloc
    reallocStack(cValues);
  }
}

void ensureReadSpace(bitStack *cValues) {
  if (cValues->top == 32) {
    cValues->index++;
    cValues->top = 0;
  }
}
