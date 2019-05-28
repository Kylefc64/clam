#include "Utils.h"

#include <tomcrypt.h>
#include <sys/types.h>
#include <random>

/**
  Uses the mersenne twister algorithm to generate a 'size'-byte random
  number and stores the result in the 'result' buffer. Assumes that
  'size' is some multiple of 4-bytes and that 'result' is large enough
  to hold the result.
*/
void Utils::genRand(unsigned char *result, int size) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
  uint32_t *resultWriter = (uint32_t *)result;
  for (int i = 0; i < size / 4; ++i) {
    resultWriter[i] = dist(mt);
  }
}

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

void Utils::concatArr(const unsigned char *buffer1, const unsigned char *buffer2, int len1, int len2, unsigned char *output) {
	for (int i = 0; i < len1; i++) {
		output[i] = buffer1[i];
	}
	for (int j = 0; j < len2; j++) {
		output[len1+j] = buffer2[j];
	}
}

bool Utils::verifyKey(std::string vaultKey, const unsigned char *salt, const unsigned char *correctHash) {
	unsigned char providedKeyHash[SKEY_LENGTH];
	unsigned char unsaltedKeyHash[SKEY_LENGTH];
	unsigned char concatBuffer[SKEY_LENGTH * 2];
	Utils::sha256(unsaltedKeyHash, (unsigned char *)vaultKey.c_str(), vaultKey.size());
	Utils::concatArr(unsaltedKeyHash, salt, SKEY_LENGTH, SKEY_LENGTH, concatBuffer);
	Utils::sha256(providedKeyHash, concatBuffer, SKEY_LENGTH * 2);
	return Utils::contentsEqual(providedKeyHash, correctHash, SKEY_LENGTH);
}