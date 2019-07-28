#ifndef VAULT_METADATA_H
#define VAULT_METADATA_H

#include <vector>
#include <string>

#include "Vault.h"

struct VaultInfo {
    unsigned char vaultSkeyHash[SKEY_LENGTH]; // vaultSkeyHash = sha256(sha256(skey) + vaultSkeySalt)
    unsigned char vaultSkeySalt[SKEY_LENGTH];
    std::string vaultName;
};

class VaultMetadata {
public:
    void initialize();
    void writeVaultMetaData();
    void addVault(const std::string &vaultName, const std::string &vaultKey);
    bool empty() const;
    unsigned int size() const;
    VaultInfo& activeVaultInfo();
    void listVaultNames() const;
    void updateActiveVaultKey(const std::string &oldVaultKey, const std::string &newVaultKey);
    void switchActiveVault(const std::string &vaultKey, const std::string &vaultToSwitchToName);
    void deleteVault(const std::string &vaultKey, const std::string &vaultToDeleteName);
    static bool validateKey(std::string key, const unsigned char *salt, const unsigned char *hash);
private:
    void readVaultMetaData();
    void updateVaultInfo(VaultInfo &updatedVaultInfo);
    std::vector<VaultInfo> vaultMetaData;
};

#endif
