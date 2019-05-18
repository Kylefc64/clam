#ifndef VAULT_H
#define VAULT_H

#include "Account.h"

#include <string>
#include <vector>

class Vault {
public:
	Vault(const std::string &vaultName, const std::string &vaultKey, bool create);
	~Vault();
	const Account& getAccount(const std::string &tag) const;
	void updateAccount(Account account);
	bool exists(std::string accountTag);
private:
	std::string vaultName;
	std::string vaultKey;
	std::vector<Account> accounts; // decrypted accounts
	void write() const;
	bool contentsEqual(const unsigned char *buffer1, const unsigned char *buffer2, uint32_t size) const;
}

#endif
