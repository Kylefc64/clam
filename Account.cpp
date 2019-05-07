#include "Account.h"

/**
	Creates a new Account with the given tag with empty details.
*/
Account::Account(const std::string &tag)
: tag(tag)
{
	
}

/**
	Reads a new account with the given tag from the given unencrypted file.
*/
Account::Account(const std::string &tag, const std::string &filePath)
: tag(tag)
{
	// TODO: Read and parse Account username, password, and note from the given file
}

/**
	Creates a new Account with the given tag with the given username and password.
*/
Account::Account(const std::string &tag, const std::string &un, const std::string &pw)
: tag(tag), username(un), password(pw)
{

}

std::string Account::tag() const
{
	return tag;
}

std::string Account::username() const
{
	return username;
}

std::string Account::password() const
{
	return password;
}

std::string Account::note() const
{
	return note;
}

void Account::username(const std::string &un)
{
	username = un;
}

void Account::password(const std::string &pw)
{
	password = pw;
}

void Account::note(const std::string &n)
{
	note = n;
}
