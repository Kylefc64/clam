#include <iostream>

#include "CommandLineParser.h"
#include "Vault.h"
#include <sys/stah.h>
#include <sys/types.h>

Vault initialize();
void processVaultCommand(const CommandLineParser& args);
void processAccountCommand(const CommandLineParser& args);

int main(int argc, char *argv[]) {
	Vault activeVault = initialize();

	CommandLineParser cmdLineParser(argc, argv);
	if (cmdLineParser.containsArg("-v")) {
		// This is a vault command
		processVaultCommand(cmdLineParser);
	} else {
		// This is a command that pertains to some account (or accounts) in the currently active vault
		processAccountCommand(cmdLineParser);
	}
}

Vault initialize() {
	struct stat info;
	if (stat("config", &info) != 0) {
		// Create config file and default vault
	}
}

/**
	Process a command that pertains to an entire vault
*/
void processVaultCommand(const CommandLineParser& args)
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
void processAccountCommand(const CommandLineParser& args)
{
	std::string vaultKey = arg.getArg("-k");
	if (vaultKey == "") {
		std::cout << "Error: Vault key must be provided to access the active vault's accounts" << std::endl;
		return;
	}
	if (args.containsArg("-p")) {

	} else if (args.containsArg("-c")) {

	} else if (arg.containsArg("-u")) {

	} else if (arg.containsArg("-a")) {

	} else {
		std::cout << "Error: Invalid account command\n"
			<< "Valid command options are: -p, -c, -u, -a" << std::endl;
			return;
	}
}
