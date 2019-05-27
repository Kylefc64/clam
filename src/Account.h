#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>

class Account {
public:
	Account(const std::string &tag);
	Account(const std::string &tag, const std::string &filePath);
	Account(const std::string &tag, const std::string &un, const std::string &pw);
	Account(unsigned char **serializedAccount);
	std::string getTag() const;
	std::string getUsername() const;
	std::string getPassword() const;
	std::string getNote() const;
	void setUsername(const std::string &un);
	void setPassword(const std::string &pw);
	void setNote(const std::string &n);
	std::vector<uint8_t> serialize() const;
	void wipeSensitiveData();
private:
	std::string tag;
	std::string username;
	std::string password;
	std::string note;
};

#endif
