#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <fstream>
#include <cstring>

#include "VaultManager.h"
#include "Utils.h"

VaultManager::VaultManager() {
    initialize();
}

bool VaultManager::empty() const {
    return vaultMetaData.empty();
}

unsigned int VaultManager::size() const {
    return vaultMetaData.size();
}

VaultInfo& VaultManager::activeVaultInfo() {
    return vaultMetaData[0];
}

void VaultManager::addVault(const std::string &vaultName, const std::string &vaultKey) {
    VaultInfo newVaultInfo;
    newVaultInfo.vaultName = vaultName;

    // Error if the vault already exists:
    for (int i = 1; i < vaultMetaData.size(); i++) {
        if (vaultMetaData[i].vaultName == newVaultInfo.vaultName) {
            std::cout << "Error: A vault with the given name already exists." << std::endl;
            return;
        }
    }
    
    Vault newVault(newVaultInfo.vaultName, vaultKey, true);
    newVault.writeVault();

    // Generate pw hash and salt for the new vault

    // Compute random salt:
    Utils::genRand(newVaultInfo.vaultSkeySalt, SKEY_LENGTH);

    // Generate pw hash
    unsigned char newVaultSkey[SKEY_LENGTH];
    unsigned char concatBuffer[SKEY_LENGTH * 2];
    Utils::sha256(newVaultSkey, (unsigned char *)vaultKey.c_str(), vaultKey.size());
    Utils::concatArr(newVaultSkey, newVaultInfo.vaultSkeySalt, SKEY_LENGTH, SKEY_LENGTH, concatBuffer);
    Utils::sha256(newVaultInfo.vaultSkeyHash, concatBuffer, SKEY_LENGTH * 2);

    // Add new vault metadata to vector of vault metadatam:
    vaultMetaData.push_back(newVaultInfo);

    Utils::debugPrint(std::cout, newVaultInfo.vaultName + " new vault name \n");
    Utils::debugPrint(std::cout, "creating new vault using key = " + vaultKey + "\n");
    Utils::debugPrint(std::cout, std::to_string(newVaultInfo.vaultSkeyHash[0]) + " new vault hash \n");
    Utils::debugPrint(std::cout, std::to_string(newVaultInfo.vaultSkeySalt[0]) + " new vault salt \n");

    // Write the updated vault metadata to disk:
    writeVaultMetaData();
}

void VaultManager::updateActiveVaultKey(const std::string &oldVaultKey, const std::string &newVaultKey) {
    const unsigned char* activeVaultHash = vaultMetaData[0].vaultSkeyHash;
    unsigned char* activeVaultSalt = vaultMetaData[0].vaultSkeySalt;
    std::string activeVaultName = vaultMetaData[0].vaultName;

    // Verify that vaultKey is correct and report error and exit if not:
    if (!validateKey(oldVaultKey, activeVaultSalt, activeVaultHash)) {
        return;
    }

    // Compute and store new random salt:
    unsigned char newSalt[SKEY_LENGTH];
    Utils::genRand(newSalt, SKEY_LENGTH);
    std::memcpy(activeVaultSalt, newSalt, SKEY_LENGTH);
    
    // Re-encrypt active Vault with new key:
    Vault activeVault(activeVaultName, oldVaultKey);
    activeVault.updateKey(newVaultKey);
    activeVault.writeVault();

    // fixed: added changes to metadata after updating vault key
    VaultInfo upadatedVaultInfo;
    upadatedVaultInfo.vaultName = activeVaultName;
    std::memcpy(upadatedVaultInfo.vaultSkeySalt, newSalt, SKEY_LENGTH);

    // calcuate new hash for updated metadata and fill upadatedVaultInfo.vaultSkeyHash
    unsigned char concatBuffer[SKEY_LENGTH * 2];
    Utils::sha256(upadatedVaultInfo.vaultSkeyHash, (unsigned char *)newVaultKey.c_str(), newVaultKey.size());
    Utils::concatArr(upadatedVaultInfo.vaultSkeyHash, newSalt, SKEY_LENGTH, SKEY_LENGTH, concatBuffer);
    Utils::sha256(upadatedVaultInfo.vaultSkeyHash, concatBuffer, SKEY_LENGTH * 2);

    updateVaultInfo(upadatedVaultInfo);
}

void VaultManager::switchActiveVault(const std::string &vaultKey, const std::string &vaultToSwitchToName) {
    if (vaultMetaData.empty()) {
        // Error if there is no active vault:
        std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
        return;
    } else if (vaultMetaData[0].vaultName == vaultToSwitchToName) {
        std::cout << "Error: " + vaultToSwitchToName + " is already the active vault." << std::endl;
        return;
    }

    for (int i = 1; i < vaultMetaData.size(); ++i) {
        // Find the vault to switch to
        if (vaultMetaData[i].vaultName == vaultToSwitchToName) {
            // Validate key
            if (!validateKey(vaultKey, vaultMetaData[i].vaultSkeySalt, vaultMetaData[i].vaultSkeyHash)) {
                return;
            }
            // Swap positions of current active vault and desired active vault
            std::swap(vaultMetaData[0], vaultMetaData[i]);

            // Write updates to the VaultInfo vector to disk:
            writeVaultMetaData();
            std::cout << "Switched to vault " + vaultMetaData[0].vaultName + "."<< std::endl;
            return;
        }
    }

    // No match found, no vault with name equal to vaultToSwitchToName exist in meta file
    std::cout << "Error: No vault with the name of \"" + vaultToSwitchToName + "\" exist" << std::endl;
}

void VaultManager::deleteVault(const std::string &vaultKey, const std::string &vaultToDeleteName) {
    std::string activeVaultName = vaultMetaData[0].vaultName;
    if (vaultToDeleteName == "") {
        std::cout << "Error: You must specify which vault to delete." << std::endl;
        return;
    } else if (vaultToDeleteName == activeVaultName) {
        std::cout << "Error: You cannot delete a vault that is currently active." << std::endl;
        return;
    }

    std::string filePathToRemove = "vaults/" + vaultToDeleteName;

    // Search in vaultMetaData for VaultInfo to delete
    for (int i = 1; i < vaultMetaData.size(); i++) { 
        if (vaultMetaData[i].vaultName == vaultToDeleteName) {
            // Verify that vaultKey is correct and report error and exit if not
            if (!validateKey(vaultKey, vaultMetaData[i].vaultSkeySalt, vaultMetaData[i].vaultSkeyHash)) {
                return;
            } 
            // Remove the vault to delete's name from the meta/meta file
            vaultMetaData.erase(vaultMetaData.begin() + i);
            // Update vault metadata file:
            writeVaultMetaData();
            // Remove the vault file in the 'vaults' directory:
            std::remove(filePathToRemove.c_str());
            std::cout << vaultToDeleteName + " has been deleted."<< std::endl;
            return;
        }
    }
    
    // No match found, no vault with name equal to vaultToDeleteName exist in meta file
    std::cout << "Error: No vault with the name of \"" + vaultToDeleteName + "\" exist" << std::endl;
    return;
}

void VaultManager::listVaultNames() const {
    for (int i = 0; i < vaultMetaData.size(); i++) {
        std::cout << vaultMetaData[i].vaultName << std::endl;
    }
}

/**
    Returns true if the provided vaultKey verifies using the given salt and salted hash values.
    Returns False and reports an error otherwise.
*/
bool VaultManager::validateKey(std::string key, const unsigned char *salt, const unsigned char *hash) {
    if (!Utils::verifyKey(key, salt, hash, SKEY_LENGTH)) {
        std::cout << "Error: The provided vault key is incorrect." << std::endl;
        return false;
    }
    return true;
}

/**
    Reads all vault metadata from the meta/meta file and creates the meta and vaults
    directories if they do not yet exist. Stores all vault metadata in the 'vaultMetaData'
    vector.
*/
void VaultManager::initialize() {
    Utils::debugPrint(std::cout, "Entered initialize\n");

    struct stat info;
    if (((stat("meta", &info) != 0)) ||
        ((stat("vaults", &info) != 0))) {
        // meta or vaults directories do not exist:
        
        // Create empty meta and vaults directories:
        system("mkdir meta/");
        system("mkdir vaults/");
        //system("touch meta/meta");
        //system("touch vaults/default");
    } else if ((stat("meta/meta", &info) != 0)) {
        // meta/meta file does not exist:
    } else {
        // Read meta/meta file:
        readVaultMetaData();
    }
}

/**
    Read all of the vault metadata into the in-memory vaultMetaData vector.
    The metadata in the meta file is of the following format:

    uint32: n = number of vaults
    
    uint32: s = size of the active vault's name (in bytes)
    s bytes: name = active vault's name
    32 bytes: active vault's hash = sha256(sha256(vaultKey) || salt)
    32 bytes: active vault's salt = some random 32-byte value

    <repeated n-1 times>:
        uint32: s = size of the vault's name (in bytes)
        s bytes: name = vault's name
        32 bytes: hash = sha256(sha256(vaultKey) || salt)
        32 bytes: salt = some random 32-byte value
*/
void VaultManager::readVaultMetaData() {
    Utils::debugPrint(std::cout, "Entered readVaultMetaData\n");

    std::ifstream fileStream("meta/meta");

    uint32_t numVaults;
    fileStream.read((char *)&numVaults, sizeof(numVaults));

    VaultInfo vaultInfo;
    uint32_t vaultNameSize;
    for (int i = 0; i < numVaults; ++i) {
        fileStream.read((char *)&vaultNameSize, sizeof(vaultNameSize)); // read vault name's size
        vaultInfo.vaultName.resize(vaultNameSize); // reserve vaultNameSize bytes
        fileStream.read((char *)&vaultInfo.vaultName[0], vaultNameSize); // read vaultName from file
        fileStream.read((char *)vaultInfo.vaultSkeyHash, SKEY_LENGTH);
        fileStream.read((char *)vaultInfo.vaultSkeySalt, SKEY_LENGTH);
        vaultMetaData.push_back(vaultInfo);
    }
    fileStream.close();
}

/**
    Write all of the vaultMetaData vector to the vault metadata file.
    The vault metadata is written to the file in the following format:

    uint32: n = number of vaults
    
    uint32: s = size of the active vault's name (in bytes)
    s bytes: name = active vault's name
    32 bytes: active vault's hash = sha256(sha256(vaultKey) || salt)
    32 bytes: active vault's salt = some random 32-byte value

    <repeated n-1 times>:
        uint32: s = size of the vault's name (in bytes)
        s bytes: name = vault's name
        32 bytes: hash = sha256(sha256(vaultKey) || salt)
        32 bytes: salt = some random 32-byte value
*/
void VaultManager::writeVaultMetaData() {
    Utils::debugPrint(std::cout, "Entered writeVaultMetaData\n");

    std::ofstream fileStream("meta/meta");

    uint32_t numVaults = vaultMetaData.size();
    fileStream.write((char *)&numVaults, sizeof(numVaults));

    VaultInfo vaultInfo;
    uint32_t vaultNameSize;
    for (int i = 0; i < numVaults; ++i) {
        vaultInfo = vaultMetaData[i];
        vaultNameSize = vaultInfo.vaultName.size();
        fileStream.write((char *)&vaultNameSize, sizeof(vaultNameSize)); // write vault name's size
        fileStream.write((char *)vaultInfo.vaultName.c_str(), vaultNameSize); // write vaultName to file
        fileStream.write((char *)vaultInfo.vaultSkeyHash, SKEY_LENGTH);
        fileStream.write((char *)vaultInfo.vaultSkeySalt, SKEY_LENGTH);
    }
    fileStream.close();
}

void VaultManager::updateVaultInfo(VaultInfo &updatedVaultInfo) {
    // Search in vaultMetaData for VaultInfo to update
    for (int i = 0; i < vaultMetaData.size(); i++) { 
        if (vaultMetaData[i].vaultName == updatedVaultInfo.vaultName) {
            // overwrite the metadata of the corresponding vault:
            vaultMetaData[i] = updatedVaultInfo;
            break;
        }
    }

    // Update vault metadata file:
    writeVaultMetaData();
    std::cout << "Successfully updated key for vault " + vaultMetaData[0].vaultName << std::endl;
}
