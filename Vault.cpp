#include "Vault.h"

#include <tomcrypt.h>
#include <fstream>
#include <random>
#include <cstring>

/**
	Encrypts and loads into memory the vault located at vaults/vaultName if create is false.
	Creates and writes to disk a new Vault at vaults/vaultName if create is true.
	Assumes that the Vault key is correct, that if create is false a Vault with the given
	name exists, and that if create is true a Vault with the given name does not exist.
*/
Vault::Vault(const std::string &vaultName, const std::string &vaultKey, bool create=false)
: vaultName(vaultName), vaultKey(vaultKey) {
	std::string filePath = "vaults/" + vaultName;
	std::ifstream fileStream(filePath);
	
	if (create) {
		// Create a new empty vault with the given name (assume a vault with the given name does not exist):
		// New vaults will be empty
	} else {
		// Load the vault with the given name (assume it exists):
		fileStream.seekg(0, fileStream.end());
		int fileSize = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg());
		if (fileSize == 0) {
			// Do not try to decrypt and load accounts from an empty vault:
			return;
		}

		// Compute sha512(vaultKey) (skey):
		unsigned char skey[SKEY_LENGTH];
		hash_state md;
		sha256_init(&md);
		sha256_process(&md, vaultey.c_str(), vaultKey.size());
		sha256_done(&md, skey);

		// Load 32-byte iv
		unsigned char iv[SKEY_LENGTH];
		fileStream.read(iv, SKEY_LENGTH);

		// Load remaining bytes (until EOF) into a byte array
		int ciphertextSize = fileSize - 2*SKEY_LENGTH;
		unsigned char *ciphertext = new unsigned char[ciphertextSize];
		fileStream.read(ciphertext, ciphertextSize);

		// Allocate a plaintext buffer in which to store the decrypted plaintext:
		unsigned char *plaintext = new unsigned char[ciphertextSize];

		// Use sha(vaultKey) and iv to decrypt account list byte array
		// Register twofish cipher:
		if (register_cipher(&twofish_desc) == -1) {
			std::cout << "Error registering cipher.\n" << std::endl;
			return -1;
		}

		// Initialize CTR cipher:
		int err;
		symmetric_CTR ctr;
		if ((err = ctr_start(
			find_cipher("twofish"), /* index of desired cipher */
			iv, /* the initial vector */
			skey, /* the secret key */
			SKEY_LENGTH, /* length of secret key (16 bytes) */
			0, /* 0 == default # of rounds */
			CTR_COUNTER_LITTLE_ENDIAN, /* Little endian counter */
			&ctr) /* where to store the CTR state */
			) != CRYPT_OK) {
			std::cout << "ctr_start error: " << error_to_string(err) << std::endl;
			return -1;
		}

		// Decrypt plaintext using CTR cipher:
		if ((err = ctr_encrypt(ciphertext, /* ciphertext */
			plaintext, /* plaintext */
			plaintextSize, /* length of plaintext pt */
			&ctr) /* CTR state */
			) != CRYPT_OK) {
			std::cout << "ctr_decrypt error: " << error_to_string(err) << std::endl;
			return -1;
		}

		// Load one account at a time from the decrypted byte array:
		unsigned char *plaintextIter = plaintext;
		unsigned char *plaintextEnd = plaintext + plaintextSize;
		while (plaintextIter != plaintextEnd) {
			accounts.push_back(Account(plaintextIter));
		}

		// Clean up cipher and memory:
		ctr_done(&ctr);
		zeromem(&ctr, sizeof(ctr));
		zeromem(skey, SKEY_LENGTH);
		zeromem(plaintext, plaintextSize);
		delete[] plaintext;
		delete[] ciphertext;
	}
	fileStream.close();
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

	// Compute random iv/nonce:
	unsigned char iv[SKEY_LENGTH];
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
	uint32_t *ivWriter = iv;
	for (int i = 0; i < SKEY_LENGTH / 4; ++i) {
		ivWriter[i] = dist(mt);
	}

	// Use sha(vaultKey) and iv to encrypt byte array

	// Register twofish cipher:
	if (register_cipher(&twofish_desc) == -1) {
		std::cout << "Error registering cipher.\n" << std::endl;
		return -1;
	}

	// Initialize CTR cipher:
	int err;
	symmetric_CTR ctr;
	if ((err = ctr_start(
		find_cipher("twofish"), /* index of desired cipher */
		iv, /* the initial vector */
		skey, /* the secret key */
		SKEY_LENGTH, /* length of secret key (16 bytes) */
		0, /* 0 == default # of rounds */
		CTR_COUNTER_LITTLE_ENDIAN, /* Little endian counter */
		&ctr) /* where to store the CTR state */
		) != CRYPT_OK) {
		std::cout << "ctr_start error: " << error_to_string(err) << std::endl;
		return -1;
	}

	// Allocate buffer in which to store the ciphertext:
	unsigned char *ciphertext = new unsigned char[plaintextSize];

	// Encrypt plaintext using CTR cipher:
	if ((err = ctr_encrypt(plaintext, /* plaintext */
		ciphertext, /* ciphertext */
		plaintextSize, /* length of plaintext pt */
		&ctr) /* CTR state */
		) != CRYPT_OK) {
		std::cout << "ctr_encrypt error: " << error_to_string(err) << std::endl;
		return -1;
	}

	// write sha(sha(vaultKey)), iv, and encrypted byte array to disk under vaults/vaultName
	std::string filePath = "vaults/" + vaultName;
	std::ofstream fileStream(filePath);
	fileStream.write(iv, SKEY_LENGTH);
	fileStream.write(ciphertext, plaintextSize);
	fileStream.close();

	// Clean up cipher and memory:
	ctr_done(&ctr);
	zeromem(&ctr, sizeof(ctr));
	zeromem(skey, SKEY_LENGTH);
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
