#include "Utils.h"

/**
  Returns true if the contents of buffer 1 are equal to the contents of buffer 2 and false otherwise.
  Assumes that sizeof(buffer1) == sizeof(buffer2).
*/
bool Utils::contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size) const {
  for (int i = 0; i < size; ++i) {
    if (buffer1[i] != buffer2[i]) {
      return false;
    }
  }
  return true;
}
