#include "Utils.h"
#include <tomcrypt.h>

/**
  Returns true if the contents of buffer 1 are equal to the contents of buffer 2 and false otherwise.
  Assumes that sizeof(buffer1) == sizeof(buffer2).
*/
bool Utils::contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size) {
  for (int i = 0; i < size; ++i) {
    if (buffer1[i] != buffer2[i]) {
      return false;
    }
  }
  return true;
}

/**
  Sets the contents of the given string to 0.
*/
void Utils::clearString(std::string &str) {
  for (int i = 0; i < str.size(); ++i) {
    str[i] = (char)0;
  }
}

/**
  Computes a sha256 of the input and stores the result in result.
*/
void Utils::sha256(unsigned char *result, const unsigned char *input, unsigned long inputSize) {
  hash_state md;
  sha256_init(&md);
  sha256_process(&md, input, inputSize);
  sha256_done(&md, result);
}

void Utils::concatArr(const unsigned char *buffer1, const unsigned char *buffer2, int len1, int len2, const unsigned char *output) {
	for (int i = 0; i < len1; i++) {
		output[i] = buffer1[i];
	}
	for (int j = 0; j < len2; j++) {
		output[len1+j] = buffer2[j];
	}
} 