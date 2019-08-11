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

class VaultManager {
public:
    VaultManager(const std::string &metadataFilePath, const std::string &vaultDir);
    const std::string& getVaultDir() const;
    bool empty() const;
    size_t size() const;
    VaultInfo& activeVaultInfo();
    void addVault(const std::string &vaultName, const std::string &vaultKey);
    void updateActiveVaultKey(const std::string &oldVaultKey, const std::string &newVaultKey);
    void switchActiveVault(const std::string &vaultKey, const std::string &vaultToSwitchToName);
    void deleteVault(const std::string &vaultKey, const std::string &vaultToDeleteName);
    void listVaultNames() const;
    static bool validateKey(std::string key, const unsigned char *salt, const unsigned char *hash);
private:
    bool checkIfEmpty() const;
    void initialize();
    void readVaultMetaData();
    void writeVaultMetaData();
    void updateVaultInfo(VaultInfo &updatedVaultInfo);

    std::vector<VaultInfo> vaultMetaData;
    const std::string metadataFilePath;
    const std::string vaultDir;
};

#endif
