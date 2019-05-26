#ifndef UTILS_H
#define UTILS_H

class Utils {
  public:
      static void genRand(unsigned char *result, int size);
      static bool contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size);
      static void clearString(std::string &str);
      static void sha256(unsigned char *result, const unsigned char *input, unsigned long inputSize);
};

#endif
