#include <iostream>

#include "CommandLineParser.h"
#include "Vault.h"
#include <sys/stah.h>
#include <sys/types.h>
#include <string>

std::string initialize();
void processVaultCommand(const CommandLineParser& args, const std::string &activeVault);
void processAccountCommand(const CommandLineParser& args, const std::string &activeVault);
void processAccountPrintCommand(const CommandLineParser& args, const std::string &activeVault);
void processAccountClipCommand(const CommandLineParser& args, const std::string &activeVault);
void processAccountUpdateCommand(const CommandLineParser& args, const std::string &activeVault);
void processAccountAddCommand(const CommandLineParser& args, const std::string &activeVault);

int main(int argc, char *argv[]) {
	std::string activeVault = initialize();

	CommandLineParser cmdLineParser(argc, argv);
	if (cmdLineParser.containsArg("-v")) {
		// This is a vault command
		processVaultCommand(cmdLineParser, activeVault);
	} else {
		// This is a command that pertains to some account (or accounts) in the currently active vault
		processAccountCommand(cmdLineParser, activeVault);
	}
}

std::string initialize() {
	struct stat info;
	if (stat("config", &info) != 0) {
		// Create config file and default vault
	}
}

/**
	Process a command that pertains to an entire vault
*/
void processVaultCommand(const CommandLineParser& args, const std::string &activeVault)
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
void processAccountCommand(const CommandLineParser& args, const std::string &activeVault)
{
	std::string vaultKey = arg.getArg("-k");
	if (vaultKey == "") {
		std::cout << "Error: Vault key must be provided to access the active vault's accounts" << std::endl;
		return;
	}
	
	Vault activeVault(activeVault, vaultKey);
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

void processAccountPrintCommand(const CommandLineParser& args, const std::string &activeVault)
{
	if (args.containsArg("-l")) {
		// List all accounts in the active vault
		return;
	}

	std::string accountName = args.getArg("-n");
	if (args.containsArg("-un")) {
		std::string username = args.getArg("-un");
		// Update the username of the given account
	} else if (args.containsArg("-pw")) {
		std::string password = args.getArg("-pw");
		// Update the password of the given account
	} else if (args.containsArg("-note")) {
		std::string note = args.getArg("-note");
		// Update the note of the given account
	} else {
		// Print all details of the given account
	}
}

void processAccountClipCommand(const CommandLineParser& args, const std::string &activeVault)
{
	std::string accountName = args.getArg("-n");
	if (args.containsArg("-un")) {
		// Clip decrypted username of the given account
	} else if (args.containsArg("-pw")) {
		// Clip decrypted password of the given account
	} else {
		// Error
	}
}

void processAccountUpdateCommand(const CommandLineParser& args, const std::string &activeVault)
{
	std::string accountName = args.getArg("-n");
	if (args.containsArg("-un")) {
		std::string username = args.getArg("-un");
		// Update the username of the given account
	} else if (args.containsArg("-pw")) {
		std::string password = args.getArg("-pw");
		// Update the password of the given account
	} else if (args.containsArg("-note")) {
		std::string note = args.getArg("-note");
		// Update the note of the given account
	} else if (args.containsArg("-f")) {
		std::string filePath = args.getArg("-f");
		// Update all details of the given account
	} else {
		// Error
	}
}

void processAccountAddCommand(const CommandLineParser& args, const std::string &activeVault)
{
	std::string accountName = args.getArg("-n");
	if (args.containsArg("-f")) {
		std::string filePath = args.getArg("-f");
		// Read the new account from the specified file
	} else if (args.containsArg("-un") && args.containsArg("-pw")) {
		std::string username = args.getArg("-un");
		std::string password = args.getArg("-pw");
		// Create a new account with the given username and password
	} else {
		// Create a new account with no details
	}
}
