#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils {
  public:
      static void genRand(unsigned char *result, int size);
      static bool contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size);
      static void clearString(std::string &str);
      static void sha256(unsigned char *result, const unsigned char *input, unsigned long inputSize);
      static void concatArr(const unsigned char *buffer1, const unsigned char *buffer2, int len1, int len2, unsigned char *output);
};

#endif
