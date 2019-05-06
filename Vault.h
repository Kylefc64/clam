#ifndef VAULT_H
#define VAULT_H

#include <string>

class Vault {
public:
	Vault(const std::string &fileName);
	void write();
	std::string encryptionKeyHash();
	
private:
	std::string nonce;
	std::string mac;
	std::string keyHash;
}

#endif
