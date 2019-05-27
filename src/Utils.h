#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>

class Utils {
 public:
	static void genRand(unsigned char *result, int size);
	static bool contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size);
	static void clearString(std::string &str);
	static void sha256(unsigned char *result, const unsigned char *input, unsigned long inputSize);
	static void concatArr(const unsigned char *buffer1, const unsigned char *buffer2, int len1, int len2, unsigned char *output);
	static void ctrEncrypt(const unsigned char *plaintext, unsigned char *ciphertext, int plaintextSize, unsigned char *iv, const unsigned char *skey, int skeySize);
	static void ctrDecrypt(const unsigned char *ciphertext, unsigned char *plaintext, int ciphertextSize, const unsigned char *iv, const unsigned char *skey, int skeySize);
	static void debugEnable();
	static void debugDisable();
	static void debugPrint(std::ostream &outputStream, const std::string& str);
private:
	static bool debug;
};

#endif
