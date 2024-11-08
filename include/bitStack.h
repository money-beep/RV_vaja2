#pragma once

typedef struct {
  long *bits; // is used to create a dynamic stack of bits
  int top;    // used for tracking bit usage
  int index;  // used for tracking index of bits array
  int size;   // size tracking size
} bitStack;

void reallocStack(bitStack *cValues);

void encode(bitStack *cValues, int needed_bits, long value);

void ensureSpace(bitStack *cValues);

void ensureReadSpace(bitStack *cValues);
