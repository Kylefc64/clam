#ifndef VAULT_H
#define VAULT_H

#include "Account.h"

#include <string>
#include <vector>
#include <iostream>
#include <optional>

#define SKEY_LENGTH 32 // symmetric key length in bytes (256 bits)

class Vault {
public:
    Vault(const std::string &vaultDir, const std::string &vaultName, const std::string &vaultKey);
    ~Vault();
    void printTags(std::ostream &outputStream) const;
    void printInfo(std::ostream &outputStream) const;
    std::optional<Account *> getAccount(const std::string &tag);
    void addAccount(Account account);
    void removeAccount(const std::string& tag);
    void writeVault() const;
    void updateKey(const std::string &newKey);
    std::string getVaultName() const;
    std::string getVaultKey() const;
private:
    bool exists(const std::string &tag) const;
    void notExistsError() const;
    void existsError() const;
    std::string vaultName;
    std::string vaultKey;

    // Do not store Accounts as a map for security reasons...
    std::vector<Account> accounts; // decrypted accounts
    const std::string vaultFilePath;
};

#endif
