#ifndef VAULT_H
#define VAULT_H

#include <string>

class Vault {
public:
	Vault(const std::string &fileName, const std::string &vaultKey);
	void write() const;
	const Account &getAccount(const std::string &tag) const;
private:
	std::string nonce;
	std::string mac;
	std::string encryptionKeyHash;
}

#endif
