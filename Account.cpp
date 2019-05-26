#include "Account.h"
#include "Utils.h"

#include <cstring>

/**
	Creates a new Account with the given tag with empty details.
*/
Account::Account(const std::string &tag)
: tag(tag), username(""), password(""), note("") {
	
}

/**
	Reads a new account with the given tag from the given unencrypted file.
*/
Account::Account(const std::string &tag, const std::string &filePath)
: tag(tag) {
	// TODO: Read and parse Account username, password, and note from the given file
}

/**
	Creates a new Account with the given tag with the given username and password.
*/
Account::Account(const std::string &tag, const std::string &un, const std::string &pw)
: tag(tag), username(un), password(pw), note("") {

}

/**
	Constructs an Account from a serialized Account.
	The provided pointer will be moved to point to the end
	of the loaded Account.
*/
Account::Account(unsigned char **serializedAccount)
{
	uint32_t size = **(uint32_t **)serializedAccount;
	*serializedAccount += 4; // advance past tag size
	tag.reserve(size);
	for (uint32_t i = 0; i < size; ++i)
	{
		tag += (*serializedAccount)[i];
	}
	*serializedAccount += size; // advance past tag

	size = **(uint32_t **)serializedAccount;
	*serializedAccount += 4; // advance past username size
	username.reserve(size);
	for (uint32_t i = 0; i < size; ++i)
	{
		username += (*serializedAccount)[i];
	}
	*serializedAccount += size; // advance username tag

	size = **(uint32_t **)serializedAccount;
	*serializedAccount += 4; // advance past password size
	password.reserve(size);
	for (uint32_t i = 0; i < size; ++i)
	{
		password += (*serializedAccount)[i];
	}
	*serializedAccount += size; // advance password tag

	size = **(uint32_t **)serializedAccount;
	*serializedAccount += 4; // advance past note size
	note.reserve(size);
	for (uint32_t i = 0; i < size; ++i)
	{
		note += (*serializedAccount)[i];
	}
	*serializedAccount += size; // advance past note
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
	uint32_t tagSize = tag.size(),
		usernameSize = username.size(),
		passwordSize = password.size(),
		noteSize = note.size();
	serialized.insert(serialized.end(), &tagSize, &tagSize + 1);
	serialized.insert(serialized.end(), tag.c_str(), tag.c_str() + tagSize);
	serialized.insert(serialized.end(), &usernameSize, &usernameSize + 1);
	serialized.insert(serialized.end(), username.c_str(), username.c_str() + usernameSize);
	serialized.insert(serialized.end(), &passwordSize, &passwordSize + 1);
	serialized.insert(serialized.end(), password.c_str(), password.c_str() + passwordSize);
	serialized.insert(serialized.end(), &noteSize, &noteSize + 1);
	serialized.insert(serialized.end(), note.c_str(), note.c_str() + noteSize);
	return serialized;
}

/**
	Wipes sensitive in-memory data.
*/
void Account::wipeSensitiveData()
{
	Utils::clearString(tag);
	Utils::clearString(username);
	Utils::clearString(password);
	Utils::clearString(note);
}
