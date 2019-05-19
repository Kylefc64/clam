#ifndef VAULT_H
#define VAULT_H

#include "Account.h"

#include <string>
#include <vector>
#include <iostream>

class Vault {
public:
	Vault(const std::string &vaultName, const std::string &vaultKey, bool create);
	~Vault();
	void printTags(std::ostream &outputStream) const;
	Account& getAccount(const std::string &tag) const;
	void addAccount(Account account);
	bool exists(const std::string &tag);
	void writeVault() const;
private:
	std::string vaultName;
	std::string vaultKey;

	// Do not store Accounts as a map for security reasons...
	std::vector<Account> accounts; // decrypted accounts

	bool contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size) const;
}

#endif
