#include "Vault.h"
#include "Utils.h"

#include <fstream>
#include <random>
#include <cstring>
#include <algorithm>

/**
    Decrypts and loads into memory the vault located at vaultDir/vaultName, if such a vault exists.
*/
Vault::Vault(const std::string &vaultDir, const std::string &vaultName, const std::string &vaultKey)
: vaultName(vaultName), vaultKey(vaultKey), vaultFilePath(vaultDir + vaultName) {
    std::ifstream fileStream(vaultFilePath);
    
    // Attempt to load the vault with the given name:
    fileStream.seekg(0, fileStream.end);
    std::streamoff fileSize = fileStream.tellg();
    fileStream.seekg(0, fileStream.beg);
    if (fileSize <= 0) {
        // Do not try to decrypt and load accounts from an empty vault:
        return;
    }

    // Compute sha512(vaultKey) (skey):
    unsigned char skey[SKEY_LENGTH];
    Utils::sha256(skey, (unsigned char *)vaultKey.c_str(), (unsigned long)vaultKey.size());

    // Load 32-byte iv
    unsigned char iv[SKEY_LENGTH];
    fileStream.read((char *)iv, SKEY_LENGTH);

    // Load remaining bytes (until EOF) into a byte array
    std::streamoff ciphertextSize = fileSize - SKEY_LENGTH;
    unsigned char *ciphertext = new unsigned char[ciphertextSize];
    fileStream.read((char *)ciphertext, ciphertextSize);

    // Allocate a plaintext buffer in which to store the decrypted plaintext:
    unsigned char *plaintext = new unsigned char[ciphertextSize];

    // Use sha(vaultKey) and iv to decrypt account list byte array
    Utils::ctrDecrypt(ciphertext, plaintext, (int)ciphertextSize, iv, skey, SKEY_LENGTH);
    
    // Load one account at a time from the decrypted byte array:
    unsigned char *plaintextIter = plaintext;
    unsigned char *plaintextEnd = plaintext + ciphertextSize;
    while (plaintextIter != plaintextEnd) {
        accounts.push_back(Account(&plaintextIter));
    }

    // Clean up memory:
    std::memset(skey, 0, SKEY_LENGTH);
    std::memset(plaintext, 0, ciphertextSize);
    delete[] plaintext;
    delete[] ciphertext;

    fileStream.close();
}

/**
    Clears all sensitive account data from memory before destroying this object.
*/
Vault::~Vault() {
    // Clear all sensitive account data from memory:
    memset((unsigned char *)vaultKey.c_str(), 0, vaultKey.size());
    for (size_t i = 0; i < accounts.size(); ++i) {
        accounts[i].wipeSensitiveData();
    }
}

/**
    Print all Account tags, separated by newlines, to the output stream.
*/
void Vault::printTags(std::ostream &outputStream) const {
    for (size_t i = 0; i < accounts.size(); ++i) {
        outputStream << accounts[i].getTag() << '\n';
    }
}

/**
    Print all nicely formatted Account info to the output stream.
*/
void Vault::printInfo(std::ostream &outputStream) const {
    for (size_t i = 0; i < accounts.size(); ++i) {
        outputStream << "Account " << i << " tag: " << accounts[i].getTag() << '\n'
            << "Account " << i << " username: " << accounts[i].getUsername() << '\n'
            << "Account " << i << " password: " << accounts[i].getPassword() << '\n'
            << "Account " << i << " note: " << accounts[i].getNote() << '\n';
    }
}

/**
    Returns a reference to the Account labeled 'tag,' or returns
    std::nullopt if an account with the given tag does not exist.
*/
std::optional<Account *> Vault::getAccount(const std::string &tag) {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].getTag() == tag) {
            return &accounts[i];
        }
    }

    notExistsError();
    return std::nullopt;
}

/**
    Adds the given Account to the vault, or prints an error
    if an account with the given tag already exists.
*/
void Vault::addAccount(Account account) {
    if (accounts.end() != 
            std::find_if(accounts.begin(),
            accounts.end(),
            [account](const Account& a) { return a.getTag() == account.getTag(); })) {
        existsError();
    } else {
        accounts.push_back(account);
    }
}

/**
    Removes the account with the given tag from the
    vault, or prints an error if it does not exist.
*/
void Vault::removeAccount(const std::string& tag) {
    auto it = std::remove_if(
                       accounts.begin(),
                       accounts.end(),
                       [tag](Account a) { return a.getTag() == tag; });

    if (it == accounts.end()) {
        notExistsError();
    } else {
        accounts.erase(it);
    }
}

/**
    Encrypts and writes all Accounts in this Vault to disk at vaultFilePath.
*/
void Vault::writeVault() const {
    if (accounts.empty()) {
        return;
    }

    // Create a byte array from list of accounts:
    std::vector<uint8_t> serializedAccountList;
    std::vector<uint8_t> serializedAccount;
    for (size_t i = 0; i < accounts.size(); ++i) {
        serializedAccount = accounts[i].serialize();
        serializedAccountList.insert(serializedAccountList.end(),
                                     serializedAccount.data(),
                                     serializedAccount.data() + serializedAccount.size());
    }

    size_t plaintextSize = serializedAccountList.size();
    unsigned char *plaintext = (unsigned char *)serializedAccountList.data();

    // Allocate buffer in which to store the ciphertext:
    unsigned char *ciphertext = new unsigned char[plaintextSize];

    // Compute sha512(vaultKey) (skey):
    unsigned char skey[SKEY_LENGTH];
    Utils::sha256(skey, (unsigned char *)vaultKey.c_str(), (unsigned long)vaultKey.size());

    // Allocate array in which to store IV/nonce:
    unsigned char iv[SKEY_LENGTH];

    // Use sha256(vaultKey) as skey to encrypt byte array:
    Utils::ctrEncrypt(plaintext, ciphertext, (int)plaintextSize, iv, skey, SKEY_LENGTH);

    // write sha(sha(vaultKey)), iv, and encrypted byte array to disk under vaults/vaultName
    std::ofstream fileStream(vaultFilePath);
    fileStream.write((char *)iv, SKEY_LENGTH);
    fileStream.write((char *)ciphertext, plaintextSize);
    fileStream.close();

    // Clean up memory:
    std::memset(skey, 0, SKEY_LENGTH);
    delete[] ciphertext;
}

void Vault::updateKey(const std::string &newKey) {
    vaultKey = newKey;
}

std::string Vault::getVaultName() const {
    return vaultName;
}

std::string Vault::getVaultKey() const {
    return vaultKey;
}

/**
    Returns true if an Account with the given tag exists and false otherwise.
*/
bool Vault::exists(const std::string &tag) const {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].getTag() == tag) {
            return true;
        }
    }

    return false;
}

/**
    Prints an error to the console indicating that a specified account does not exist.
*/
void Vault::notExistsError() const {
    std::cout << "Error: The specified account does not exist. You may create an account using the -a option." << std::endl;
}

/**
    Prints an error to the console indicating that a specified account already exists.
*/
void Vault::existsError() const {
    std::cout << "Error: There already exists an account with the specified name." << std::endl;
}
