#ifndef VAULT_H
#define VAULT_H

#include "Account.h"

#include <string>
#include <vector>

class Vault {
public:
	Vault(const std::string &fileName, const std::string &vaultKey, bool create);
	const Account& getAccount(const std::string &tag) const;
	void updateAccount(Account account);
	bool exists(std::string accountTag);
private:
	std::string nonce;
	std::string mac;
	std::string encryptionKeyHash;
	std::vector<Account> accounts; // decrypted accounts
	void write() const;
}

#endif
