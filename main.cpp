#include <iostream>

#include "CommandLineParser.h"
#include "Vault.h"
#include <sys/stah.h>
#include <sys/types.h>
#include <string>

std::string initialize();
void processVaultCommand(const CommandLineParser& args, const std::string &activeVaultName);
void processAccountCommand(const CommandLineParser& args, const std::string &activeVaultName);
void processAccountPrintCommand(const CommandLineParser& args, const Vault &activeVault);
void processAccountClipCommand(const CommandLineParser& args, const Vault &activeVault);
void processAccountUpdateCommand(const CommandLineParser& args, Vault &activeVault);
void processAccountAddCommand(const CommandLineParser& args, Vault &activeVault);

int main(int argc, char *argv[]) {
	std::string activeVaultName = initialize();

	CommandLineParser cmdLineParser(argc, argv);
	if (cmdLineParser.containsArg("-v")) {
		// This is a vault command
		processVaultCommand(cmdLineParser, activeVaultName);
	} else {
		// This is a command that pertains to some account (or accounts) in the currently active vault
		processAccountCommand(cmdLineParser, activeVaultName);
	}
}

std::string initialize() {
	struct stat info;
	if ((stat("meta", &info) != 0) || (info.st_mode & S_IFDIR)) {
		// Create meta dir/file and vaults dir & default vault file
		system("mkdir meta/");
		//system("touch meta/meta");
	}

	if ((stat("vaults", &info) != 0) || (info.st_mode & S_IFDIR)) {
		// Create meta dir/file and vaults dir & default vault file
		system("mkdir vaults/");
		//system("touch vaults/default");
	}
}

/**
	Process a command that pertains to an entire vault
*/
void processVaultCommand(const CommandLineParser& args, const std::string &activeVaultName)
{
	std::string metaCommand = args.getArg("-v");
	if (metaCommand == "add") {

	} else if (metaCommand == "update") {

	} else if (metaCommand == "switch") {

	} else if (metaCommand == "delete") {

	} else if (metaCommand == "list") {

	} else {
		std::cout << "Error: Invalid vault command\n"
			<< "Valid commands are: add, update, switch, delete, list" << std::endl;
		return;
	}
}

/**
	Process a command that pertains to some account (or accounts) in the currently active vault
*/
void processAccountCommand(const CommandLineParser& args, const std::string &activeVaultName)
{
	std::string vaultKey = arg.getArg("-k");
	if (vaultKey == "") {
		std::cout << "Error: Vault key must be provided to access the active vault's accounts" << std::endl;
		return;
	}

	// Attempt to load and decrypt vault:
	Vault activeVault(activeVaultName, vaultKey);
	if (args.containsArg("-p")) {
		processAccountPrintCommand(cmdLineParser, activeVault);
	} else if (args.containsArg("-c")) {
		processAccountClipCommand(cmdLineParser, activeVault);
	} else if (arg.containsArg("-u")) {
		processAccountUpdateCommand(cmdLineParser, activeVault);
	} else if (arg.containsArg("-a")) {
		processAccountAddCommand(cmdLineParser, activeVault);
	} else {
		std::cout << "Error: Invalid account command\n"
			<< "Valid command options are: -p, -c, -u, -a" << std::endl;
			return;
	}
}

/**
	Processes a print command. Assumes the active vault has successfully been decrypted.
*/
void processAccountPrintCommand(const CommandLineParser& args, const Vault &activeVault)
{
	if (args.containsArg("-l")) {
		activeVault.printTags(std::cout);
		return;
	}

	std::string accountName = args.getArg("-n");
	if (args.containsArg("-un")) {
		std::cout << activeVault.getAccount(accountName).username() << std::endl;
	} else if (args.containsArg("-pw")) {
		std::cout << activeVault.getAccount(accountName).password() << std::endl;
	} else if (args.containsArg("-note")) {
		std::cout << activeVault.getAccount(accountName).note() << std::endl;
	} else {
		const Account account = activeVault.getAccount(accountName);
		std::cout << "un=" << account.username() << '\n'
			<< "pw=" << account.password() << '\n'
			<< "note=" << account.note() << std::endl;
	}
}

/**
	Processes a clip command. Assumes the active vault has successfully been decrypted.
*/
void processAccountClipCommand(const CommandLineParser& args, const Vault &activeVault)
{
	std::string accountName = args.getArg("-n");
	if (args.containsArg("-un")) {
		// TODO: Clip decrypted username of the given account
	} else if (args.containsArg("-pw")) {
		// TODO: Clip decrypted password of the given account
	} else {
		// TODO: Error
	}
}

/**
	Processes an update command. Assumes the active vault has successfully been decrypted.
*/
void processAccountUpdateCommand(const CommandLineParser& args, Vault &activeVault)
{
	std::string accountName = args.getArg("-n");
	if (!activeVault.exists(accountName))
	{
		// TODO: Error
		return;
	}

	if (args.containsArg("-un")) {
		std::string username = args.getArg("-un");
		// Update the username of the given account
		Account account = activeVault.getaccount(accountName);
		account.username = username;
		activeVault.writeVault();
	} else if (args.containsArg("-pw")) {
		std::string password = args.getArg("-pw");
		// Update the password of the given account
		Account account = activeVault.getaccount(accountName);
		account.password = password;
		activeVault.writeVault();
	} else if (args.containsArg("-note")) {
		std::string note = args.getArg("-note");
		// Update the note of the given account
		Account account = activeVault.getaccount(accountName);
		account.note = note;
		activeVault.writeVault();
	} else if (args.containsArg("-f")) {
		std::string filePath = args.getArg("-f");
		// Update all details of the given account
		Account account(accountName, filePath);
		activeVault.writeVault();
	} else {
		// TODO: Error
	}
}

/**
	Processes an add command. Assumes the active vault has successfully been decrypted.
*/
void processAccountAddCommand(const CommandLineParser& args, Vault &activeVault)
{
	std::string accountName = args.getArg("-n");
	if (activeVault.exists(accountName))
	{
		// TODO: Error
		return;
	}

	if (args.containsArg("-f")) {
		std::string filePath = args.getArg("-f");
		// Read the new account from the specified file
		Account account(accountName, filePath);
		activeVault.addAccount(account);
	} else if (args.containsArg("-un") && args.containsArg("-pw")) {
		std::string username = args.getArg("-un");
		std::string password = args.getArg("-pw");
		// Create a new account with the given username and password
		Account account(accountName, username, password);
		activeVault.addAccount(account);
	} else {
		// Create a new account with no details
		Account account(accountName);
		activeVault.addAccount(account);
	}
}
