#include "Utils.h"

#include <tomcrypt.h>
#include <sys/types.h>
#include <random>
#include <cstring>

bool Utils::debug = true;

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
  std::memcpy(output, buffer1, len1);
  std::memcpy(output + len1, buffer2, len2);
}

/**
  Encrypts the contents of the 'plaintext' array of size 'plaintextSize' using the symmetric key
  'skey' of size 'skeySize' and stores the result in 'ciphertext.' Generates and stores the nonce/IV
  used to encrypt the plaintext in the 'iv' array. Assumes that 'iv' and 'skey' are of the same size
  and that the 'ciphertext' array is the same size as the 'plaintext' array. Uses the CTR block cipher
  mode of encryption.
*/
void Utils::ctrEncrypt(const unsigned char *plaintext, unsigned char *ciphertext, int plaintextSize, unsigned char *iv, const unsigned char *skey, int skeySize) {
  // Generate an IV for the cipher:
  Utils::genRand(iv, skeySize);

  // Register twofish cipher:
  if (register_cipher(&twofish_desc) == -1) {
    std::cout << "Error registering cipher.\n" << std::endl;
    exit(1);
  }

  // Initialize CTR cipher:
  int err;
  symmetric_CTR ctr;
  if ((err = ctr_start(
    find_cipher("twofish"), /* index of desired cipher */
    iv, /* the initial vector */
    skey, /* the secret key */
    skeySize, /* length of secret key (16 bytes) */
    0, /* 0 == default # of rounds */
    CTR_COUNTER_LITTLE_ENDIAN, /* Little endian counter */
    &ctr) /* where to store the CTR state */
    ) != CRYPT_OK) {
    std::cout << "ctr_start error: " << error_to_string(err) << std::endl;
    exit(1);
  }

  // Encrypt plaintext using CTR cipher:
  if ((err = ctr_encrypt(plaintext, /* plaintext */
    ciphertext, /* ciphertext */
    plaintextSize, /* length of plaintext pt */
    &ctr) /* CTR state */
    ) != CRYPT_OK) {
    std::cout << "ctr_encrypt error: " << error_to_string(err) << std::endl;
    exit(1);
  }

  ctr_done(&ctr);
  zeromem(&ctr, sizeof(ctr));
}

/**
  Decrypts the contents of the 'ciphertext' array of size 'ciphertextSize' using the symmetric key
  'skey' of size 'skeySize' and the provided nonce/IV and stores the result in 'plaintext.'Assumes
  that the 'plaintext' array is the same size as the 'ciphertext' array. Uses the CTR block cipher
  mode of decryption.
*/
void Utils::ctrDecrypt(const unsigned char *ciphertext, unsigned char *plaintext, int ciphertextSize, const unsigned char *iv, const unsigned char *skey, int skeySize) {
  // Register twofish cipher:
  if (register_cipher(&twofish_desc) == -1) {
    std::cout << "Error registering cipher.\n" << std::endl;
    exit(1);
  }

  // Initialize CTR cipher:
  int err;
  symmetric_CTR ctr;
  if ((err = ctr_start(
    find_cipher("twofish"), /* index of desired cipher */
    iv, /* the initial vector */
    skey, /* the secret key */
    skeySize, /* length of secret key (16 bytes) */
    0, /* 0 == default # of rounds */
    CTR_COUNTER_LITTLE_ENDIAN, /* Little endian counter */
    &ctr) /* where to store the CTR state */
    ) != CRYPT_OK) {
    std::cout << "ctr_start error: " << error_to_string(err) << std::endl;
    exit(1);
  }

  // Decrypt plaintext using CTR cipher:
  if ((err = ctr_decrypt(ciphertext, /* ciphertext */
    plaintext, /* plaintext */
    ciphertextSize, /* length of plaintext pt */
    &ctr) /* CTR state */
    ) != CRYPT_OK) {
    std::cout << "ctr_decrypt error: " << error_to_string(err) << std::endl;
    exit(1);
  }

  ctr_done(&ctr);
  zeromem(&ctr, sizeof(ctr));
}

void Utils::debugEnable() {
  debug = true;
}

void Utils::debugDisable() {
  debug = false;
}

void Utils::debugPrint(std::ostream &outputStream, const std::string& str) {
  if (debug) {
    outputStream << str;
  }
}

bool Utils::verifyKey(std::string vaultKey, const unsigned char *salt, const unsigned char *correctHash, int keySize) {
  // Utils::debugPrint(std::cout, vaultKey + " providedkey \n");
  // Utils::debugPrint(std::cout, std::to_string(keySize) + " providedkeysize \n");
  // Utils::debugPrint(std::cout, std::to_string(32) + " correctkeysize \n");
  unsigned char providedKeyHash[keySize];
  unsigned char unsaltedKeyHash[keySize];
  unsigned char concatBuffer[keySize * 2];
  Utils::debugPrint(std::cout, std::to_string(salt[0]) + " using salt \n");
  Utils::sha256(unsaltedKeyHash, (unsigned char *)vaultKey.c_str(), vaultKey.size());
  Utils::concatArr(unsaltedKeyHash, salt, keySize, keySize, concatBuffer);
  Utils::sha256(providedKeyHash, concatBuffer, keySize * 2);
  // Utils::debugPrint(std::cout, std::to_string(providedKeyHash[0]) + " providedKeyHash \n");
  // Utils::debugPrint(std::cout, std::to_string(correctHash[0]) + " correctHash \n");
  bool success = Utils::contentsEqual(providedKeyHash, correctHash, keySize);
  // delete[] providedKeyHash;
  // delete[] unsaltedKeyHash;
  // delete[] concatBuffer;
  // https://stackoverflow.com/questions/32118545/munmap-chunk-invalid-pointer
  return success;
}