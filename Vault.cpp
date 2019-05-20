#include "Vault.h"

#include <tomcrypt.h>
#include <fstream>
#include <random>
#include <cstring>

/**
	Encrypts and loads into memory the vault located at vaults/vaultName.
	Aborts the program if the provided vaultKey is incorrect.
*/
Vault::Vault(const std::string &vaultName, const std::string &vaultKey, bool create=false) 
: vaultName(vaultName), vaultKey(vaultKey) {
	std::string filePath = "vaults/" + vaultName;
	
	if (create) {
		// Check if the given vault exists and create a new vault if it doesn't:
	} else {
		// Load the vault with the given name if it exists:
		std::ifstream fileStream(filePath);
		fileStream.seekg(0, fileStream.end());
		int fileSize = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg());

		// -----
		// TODO: Move key hash checking outside of Vault class
		// Load 32-byte encryption key hash hash
		unsigned char storedSkeyHash[SKEY_LENGTH];
		fileStream.read(storedSkeyHash, SKEY_LENGTH);
		// -----

		// Check if sha(sha(vaultKey)) equals sha(sha(encr key)) and proceed if equal

		// Compute sha512(vaultKey) (skey):
		unsigned char skey[SKEY_LENGTH];
		hash_state md;
		sha256_init(&md);
		sha256_process(&md, vaultey.c_str(), vaultKey.size());
		sha256_done(&md, skey);

		// -----
		// TODO: Move key hash checking outside of Vault class
		// Compute sha512(skey):
		unsigned char skeyHash[SKEY_LENGTH];
		sha256_init(&md);
		sha256_process(&md, skey, SKEY_LENGTH);
		sha256_done(&md, skeyHash);

		if (!contentsEqual(skeyHash, storedSkeyHash, SKEY_LENGTH)) {
			std::cout << "Error: Invalid vault key." << std::endl;
			exit(1);
		}
		// -----

		// Load 32-byte nonce
		unsigned char nonce[SKEY_LENGTH];
		fileStream.read(nonce, SKEY_LENGTH);

		// Load remaining bytes (until EOF) into a byte array
		int ciphertextSize = fileSize - 2*SKEY_LENGTH;
		unsigned char *ciphertext = new unsigned char[ciphertextSize];
		fileStream.read(ciphertext, ciphertextSize);
		fileStream.close();

		// Allocate a plaintext buffer in which to store the decrypted plaintext:
		unsigned char *plaintext = new unsigned char[ciphertextSize];

		// Use sha(vaultKey) and nonce to decrypt account list byte array
		// TODO: Use CTR instead of CBC mode?

		// Initialize symmetric_key object using skey:
		symmetric_key libtomSkey;
		cbc_setup(skey, SKEY_LENGTH, 0, &libtomSkey);

		// Compute the number of complete blocks to process:
		unsigned long completeBlocks = ciphertextSize / SKEY_LENGTH;

		// TODO: Use ciphertext stealing to decrypt plaintext that are not multiples of 32 bytes
		int success = cbc_decrypt(ciphertext, plaintext, completeBlocks, nonce, libtomSkey);
		cbc_done(libtomSkey);

		// TODO: Decrypt final partial block using EBC (see libtomcrypt doc)

		delete[] ciphertext;

		// Load one account at a time from the decrypted byte array:
		unsigned char *plaintextIter = plaintext;
		unsigned char *plaintextEnd = plaintext + plaintextSize;
		while (plaintextIter != plaintextEnd) {
			accounts.push_back(Account(plaintextIter));
		}

		delete[] plaintext;
	}
}

/**
	Clears all sensitive account data from memory before destroying this object.
*/
Vault::~Vault() {
	// Clear all sensitive account data from memory:
	memset(vaultKey.c_str(), 0, vaultKey.size());
	for (int i = 0; i < accounts.size(); ++i)
	{
		accounts[i].wipeSensitiveData();
	}
}

/**
	Print all Account tags, separated by newlines, to the output stream.
*/
void Vault::printTags(std::ostream &outputStream) const
{
	std::vector<std::string> tags;
	for (int i = 0; i < accounts.size(); ++i)
	{
		outputStream << accounts[i].tag() << '\n';
	}
}

/**
	Returns a reference to the Account labeled 'tag.'
	Assumes that an account with the given tag exists.
*/
Account& Vault::getAccount(const std::string &tag) const {
	for (int i = 0; i < accounts.size(); ++i) {
		if (accounts[i].tag() == tag) {
			return accounts[i];
		}
	}

	// TODO: Throw exception if this line is reached
	return accounts[0]; // not reached
}

/**
	Adds the given Account to the vault.
	Assumes that an Account with the same tag does not already exist in the vault.
*/
void Vault::addAccount(Account account) {
	accounts[account.tag] = account;
}

/**
	Returns true if an Account with the given tag exists and false otherwise.
*/
bool exists(const std::string &tag) {
	for (int i = 0; i < accounts.size(); ++i) {
		if (accounts[i].tag() == tag) {
			return true;
		}
	}

	return false;
}

/**
	Encrypts and writes all Accounts in this Vault to disk at vaults/vaultName.
*/
void Vault::writeVault() const {
	// Create a byte array from list of accounts:
	std::vector<uint8_t> serializedAccountList;
	std::vector<uint8_t> serializedAccount;
	for (int i = 0; i < accounts.size(); ++i)
	{
		serializedAccount = accounts[i].serialize();
		serializedAccountList.insert(serializedAccounts.end(), serializedAccount.data(), serializedAccount.data() + serializedAccount.size());
	}

	size_t plaintextSize = serializedAccountList.size();
	unsigned char *plaintext = (unsigned char *)serializedAccountList.data();

	// Compute sha512(vaultKey) (skey):
	unsigned char skey[SKEY_LENGTH];
	hash_state md;
	sha256_init(&md);
	sha256_process(&md, vaultey.c_str(), vaultKey.size());
	sha256_done(&md, skey);

	// -----
	// TODO: Move key checking outside of Vault class:
	// Compute sha512(skey):
	unsigned char skeyHash[SKEY_LENGTH];
	sha256_init(&md);
	sha256_process(&md, skey, SKEY_LENGTH);
	sha256_done(&md, skeyHash);
	// -----

	// Compute random nonce/IV:
	unsigned char iv[SKEY_LENGTH];
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
	uint32_t *ivWriter = iv;
	for (int i = 0; i < SKEY_LENGTH / 4; ++i)
	{
		ivWriter[i] = dist(mt);
	}

	// Use sha(vaultKey) and nonce to encrypt byte array
	// TODO: Use CTR instead of CBC mode?

	// Initialize symmetric_key object using skey:
	symmetric_key libtomSkey;
	cbc_setup(skey, SKEY_LENGTH, 0, &libtomSkey);

	// Compute the number of complete blocks to process:
	unsigned long completeBlocks = plaintextSize / SKEY_LENGTH;

	// Allocate buffer in which to store the ciphertext:
	unsigned char *ciphertext = new unsigned char[plaintextSize];

	// TODO: Use ciphertext stealing to encrypt plaintext that are not multiples of 32 bytes
	int success = cbc_encrypt(plaintext, ciphertext, completeBlocks, iv, libtomSkey);
	cbc_done(libtomSkey);

	// TODO: Encrypt final partial block using EBC (see libtomcrypt doc)

	// write sha(sha(vaultKey)), nonce, and encrypted byte array to disk under vaults/vaultName
	std::string filePath = "vaults/" + vaultName;
	std::ofstream fileStream(filePath);
	fileStream.write(skeyHash, SKEY_LENGTH); // TODO: Move key checking outside of Vault class
	fileStream.write(iv, SKEY_LENGTH);
	fileStream.write(ciphertext, plaintextSize);
	fileStream.close();

	delete[] ciphertext;
}

void Vault::updateKey(const std::string &newKey) {
	vaultKey = newKey
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
