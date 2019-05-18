#include "Vault.h"

#include <tomcrypt.h>
#include <fstream>

#define SKEY_LENGTH 32 // symmetric key length in bytes (256 bits)

Vault::Vault(const std::string &vaultName, const std::string &vaultKey, bool create=false) 
: vaultName(vaultName), vaultKey(vaultKey) {
	std::string filePath = "vaults/" + vaultName;
	// TODO: Check if the file already exists first
	// Assume the file doesn't exist for now...

	if (create) {
		// Check if the given vault exists and create a new vault if it doesn't:
	} else {
		// Load the vault with the given name if it exists:
		std::ifstream fileStream(filePath);
		fileStream.seekg(0, fileStream.end());
		int fileSize = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg());

		// Load 32-byte encryption key hash hash
		unsigned char storedSkeyHash[SKEY_LENGTH];
		fileStream.read(storedSkeyHash, SKEY_LENGTH);
		// Check if sha(sha(vaultKey)) equals sha(sha(encr key)) and proceed if equal

		// Compute sha512(vaultKey) (skey):
		unsigned char skey[SKEY_LENGTH];
		hash_state md;
		sha256_init(&md);
		sha256_process(&md, vaultey.c_str(), vaultKey.size());
		sha256_done(&md, skey);

		// Compute sha512(skey):
		unsigned char skeyHash[SKEY_LENGTH];
		sha256_init(&md);
		sha256_process(&md, skey, SKEY_LENGTH);
		sha256_done(&md, skeyHash);

		if (!contentsEqual(skeyHash, storedSkeyHash, SKEY_LENGTH)) {
			std::cout << "Error: Invalid vault key." << std::endl;
			exit(1);
		}



		// Load 32-byte nonce
		unsigned char nonce[SKEY_LENGTH];
		fileStream.read(nonce, SKEY_LENGTH);

		// Load remaining bytes (until EOF) into a byte array
		int ciphertextSize = fileSize - 2*SKEY_LENGTH;
		unsigned char *ciphertext = new unsigned char[ciphertextSize]; // TODO: delete mallocd buffer later
		fileStream.read(ciphertext, ciphertextSize);
		fileStream.close();

		// Prepare a plaintext buffer in which to store the decrypted plaintext:
		unsigned char *plaintext = new unsigned char[ciphertextSize]; // TODO: delete mallocd buffer later

		// Use sha(vaultKey) and nonce to decrypt account list byte array

		// Initialize symmetric_key object using skey:
		symmetric_key libtomSkey;
		cbc_setup(skey, SKEY_LENGTH, 0, &libtomSkey);

		// Compute the number of complete blocks to process:
		unsigned long completeBlocks = ciphertextSize / SKEY_LENGTH;

		// Compute nonce/IV:
		unsigned char iv[SKEY_LENGTH];
		// TODO: Compute random nonce/IV

		int success = cbc_decrypt(ciphertext, plaintext, completeBlocks, iv, libtomSkey);
		cbc_done(libtomSkey);

		// TODO: Decrypt final partial block using EBC (see libtomcrypt doc)

		delete[] ciphertext;

		// Load one account at a time from the decrypted byte array:
			// Load 4-byte account tag size
			// Load a-byte account tag
			// Load 4-byte account username size
			// Load b-byte account username
			// Load 4-byte account password size
			// Load c-byte account password
			// Load 4-byte account note size
			// Load d-byte account note
			// repeat for all accounts

		delete[] plaintext;
	}
}

Vault::~Vault() {
	// TODO: Clear all account data from memory
}

const Account& Vault::getAccount(const std::string &tag) const {

}

void Vault::updateAccount(Account account) {

}

bool exists(std::string accountTag) {

}

void Vault::write() const {
	// Create a byte array from list of accounts
	std::vector<unsigned char> serializedAccountList;
	// TODO: Serialize Account list into vector
	unsigned char *plaintext = new unsigned char[serializedAccountList.size()]; // TODO: delete mallocd buffer later

	// Compute sha512(vaultKey) (skey):
	unsigned char skey[SKEY_LENGTH];
	hash_state md;
	sha256_init(&md);
	sha256_process(&md, vaultey.c_str(), vaultKey.size());
	sha256_done(&md, skey);

	// Compute sha512(skey):
	unsigned char skeyHash[SKEY_LENGTH];
	sha256_init(&md);
	sha256_process(&md, skey, SKEY_LENGTH);
	sha256_done(&md, skeyHash);

	// Compute nonce/IV:
	unsigned char iv[SKEY_LENGTH];
	// TODO: Compute random nonce/IV

	// Use sha(vaultKey) and nonce to encrypt byte array

	// Initialize symmetric_key object using skey:
	symmetric_key libtomSkey;
	cbc_setup(skey, SKEY_LENGTH, 0, &libtomSkey);

	// Compute the number of complete blocks to process:
	unsigned long completeBlocks = serializedAccountList.size() / SKEY_LENGTH;

	// Initialize buffer in which to store the ciphertext:
	unsigned char *ciphertext = new unsigned char[serializedAccountList.size()]; // TODO: delete mallocd buffer later

	int success = cbc_encrypt(plaintext, ciphertext, completeBlocks, iv, libtomSkey);
	cbc_done(libtomSkey);

	// TODO: Encrypt final partial block using EBC (see libtomcrypt doc)

	// write sha(sha(vaultKey)), nonce, and encrypted byte array to disk under vaults/vaultName
}

/**
	Returns true if the contents of buffer 1 are equal to the contents of buffer 2 and false otherwise.
	Assumes that sizeof(buffer1) == sizeof(buffer2).
*/
bool Vault::contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size) const {
	for (int i = 0; i < size; ++i) {
		if (buffer1[i] != buffer2[i]) {
			return false;
		}
	}
	return true;
}
