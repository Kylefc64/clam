#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

class Account {
public:
	Account(const std::string &tag);
	Account(const std::string &tag, const std::string &filePath);
	Account(const std::string &tag, const std::string &un, const std::string &pw);
	std::string tag() const;
	std::string username() const;
	std::string password() const;
	std::string note() const;
	void username(const std::string &un);
	void password(const std::string &pw);
	void note(const std::string &n);
private:
	std::string tag;
	std::string username;
	std::string password;
	std::string note;
}

#endif
