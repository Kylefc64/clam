#include "Account.h"
#include "Utils.h"

#include <cstring>
#include <fstream>

/**
    Creates a new Account with the given tag with empty details.
*/
Account::Account(const std::string &tag)
: tag(tag), username(""), password(""), note("") {
    
}

/**
    Creates a new Account with the given tag, username, and password.
*/
Account::Account(const std::string &tag, const std::string &un, const std::string &pw)
: tag(tag), username(un), password(pw), note("") {

}

/**
    Constructs an Account from a serialized Account.
    The provided pointer will be moved to point to the end
    of the loaded Account.
*/
Account::Account(unsigned char **serializedAccount) {
    uint32_t size = **(uint32_t **)serializedAccount;
    *serializedAccount += 4; // advance past tag size
    tag.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
        tag += (*serializedAccount)[i];
    }
    *serializedAccount += size; // advance past tag

    size = **(uint32_t **)serializedAccount;
    *serializedAccount += 4; // advance past username size
    username.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
        username += (*serializedAccount)[i];
    }
    *serializedAccount += size; // advance username tag

    size = **(uint32_t **)serializedAccount;
    *serializedAccount += 4; // advance past password size
    password.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
        password += (*serializedAccount)[i];
    }
    *serializedAccount += size; // advance password tag

    size = **(uint32_t **)serializedAccount;
    *serializedAccount += 4; // advance past note size
    note.reserve(size);
    for (uint32_t i = 0; i < size; ++i) {
        note += (*serializedAccount)[i];
    }
    *serializedAccount += size; // advance past note
}

/**
    Reads a new account with the given tag from the given unencrypted file.
    Returns true if an account was successfully parsed from the specified
    file and false otherwise.
*/
bool Account::loadFromFile(const std::string &filePath) {
    std::ifstream fileStream(filePath);

    if (!fileStream) {
        std::cout << "Error: Failed to load account from file." << std::endl;
        return false;
    }

    if (!getline(fileStream, username)) {
        std::cout << "Error: Failed to load account from file." << std::endl;
        return false;
    }

    if (!getline(fileStream, password)) {
        std::cout << "Error: Failed to load account from file." << std::endl;
        return false;
    }

    std::string nextLine;
    while (getline(fileStream, nextLine)) {
        note += nextLine + '\n';
    }

    note.erase(note.size() - 1); // remove the trailing newline character

    fileStream.close();

    return true;
}

std::string Account::getTag() const {
    return tag;
}

std::string Account::getUsername() const {
    return username;
}

std::string Account::getPassword() const {
    return password;
}

std::string Account::getNote() const {
    return note;
}

void Account::setUsername(const std::string &un) {
    std::cout << "un: " << un << std::endl;
    username = un;
}

void Account::setPassword(const std::string &pw) {
    password = pw;
}

void Account::setNote(const std::string &n) {
    note = n;
}

/**
    Stores and returns a serialized version of this object as a byte vector.
*/
std::vector<uint8_t> Account::serialize() const {
    std::vector<uint8_t> serialized;
    uint32_t tagSize = (uint32_t)tag.size(),
        usernameSize = (uint32_t)username.size(),
        passwordSize = (uint32_t)password.size(),
        noteSize = (uint32_t)note.size();
    serialized.insert(serialized.end(), (char *)&tagSize, (char *)&tagSize + sizeof(tagSize));
    serialized.insert(serialized.end(), tag.c_str(), tag.c_str() + tagSize);
    serialized.insert(serialized.end(), (char *)&usernameSize, (char *)&usernameSize + sizeof(usernameSize));
    serialized.insert(serialized.end(), username.c_str(), username.c_str() + usernameSize);
    serialized.insert(serialized.end(), (char *)&passwordSize, (char *)&passwordSize + sizeof(passwordSize));
    serialized.insert(serialized.end(), password.c_str(), password.c_str() + passwordSize);
    serialized.insert(serialized.end(), (char *)&noteSize, (char *)&noteSize + sizeof(noteSize));
    serialized.insert(serialized.end(), note.c_str(), note.c_str() + noteSize);
    return serialized;
}

/**
    Wipes sensitive in-memory data.
*/
void Account::wipeSensitiveData() {
    Utils::clearString(tag);
    Utils::clearString(username);
    Utils::clearString(password);
    Utils::clearString(note);
}
