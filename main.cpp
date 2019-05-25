#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <vector>

#include "CommandLineParser.h"
#include "Vault.h"
#include "Utils.h"

struct VaultInfo {
	unsigned char vaultKeyHash[SKEY_LENGTH]; // vaultKeyHash = sha256(sha256(key) + vaultKeyNonce)
	unsigned char vaultKeyNonce[SKEY_LENGTH];
	std::string vaultName;
};

std::string initialize(std::vector<VaultInfo> &vaultMetaData);
void readVaultMetaData(std::vector<VaultInfo> &vaultMetaData);
void processVaultCommand(const CommandLineParser& args, const std::vector<VaultInfo> &vaultMetaData);
void processAccountCommand(const CommandLineParser& args, const std::vector<VaultInfo> &vaultMetaData);
void processAccountPrintCommand(const CommandLineParser& args, const Vault &activeVault);
void processAccountClipCommand(const CommandLineParser& args, const Vault &activeVault);
void processAccountUpdateCommand(const CommandLineParser& args, Vault &activeVault);
void processAccountAddCommand(const CommandLineParser& args, Vault &activeVault);

int main(int argc, char *argv[]) {
	std::vector<VaultInfo> vaultMetaData;

	CommandLineParser cmdLineParser(argc, argv);
	if (cmdLineParser.containsArg("-v")) {
		// This is a vault command
		processVaultCommand(cmdLineParser, vaultMetaData);
	} else {
		// This is a command that pertains to some account (or accounts) in the currently active vault
		processAccountCommand(cmdLineParser, vaultMetaData);
	}
}

void readVaultMetaData(std::vector<VaultInfo> &vaultMetaData) {
	std::ifstream fileStream("meta/meta");

	uint32_t numVaults;
	fileStream.read((char *)&numVaults, sizeof(numVaults));

	VaultInfo vaultInfo;

	for (int i = 0; i < numVaults; ++i) {
		fileStream.read((char *)vaultInfo.vaultKeyHash, SKEY_LENGTH);
		fileStream.read((char *)vaultInfo.vaultKeyNonce, SKEY_LENGTH);
		std::getline(fileStream, vaultInfo.vaultName);
		vaultMetaData.push_back(vaultInfo);
	}
}

/**
	Returns the name of the active vault if it exists, or "" if no vaults
	have been created yet. Creates meta and vaults directories if
	they do not yet exist.
*/
std::string initialize(std::vector<VaultInfo> &vaultMetaData) {
	struct stat info;
	if (((stat("meta", &info) != 0) || (info.st_mode & S_IFDIR)) ||
		((stat("vaults", &info) != 0) || (info.st_mode & S_IFDIR))) {
		// meta or vaults directories do not exist:
		
		// Create empty meta and vaults directories:
		system("mkdir meta/");
		system("mkdir vaults/");
		//system("touch meta/meta");
		//system("touch vaults/default");
		return "";
	} else if ((stat("meta/meta", &info) != 0) || (info.st_mode & S_IFDIR)) {
		// meta/meta file does not exist:
		return "";
	} else {
		// Read meta/meta file:
		readVaultMetaData(vaultMetaData);

		// // Assume that there never exists a meta/meta file containing no data or with 0 length:
		// return vaultMetaData[0].vaultName; // the active vault is first
	}
}

/**
	Process a command that pertains to an entire vault
*/
void processVaultCommand(const CommandLineParser& args, const std::vector<VaultInfo> &vaultMetaData)
{	
	const unsigned char* activeVaultHash = vaultMetaData[0].vaultKeyHash;
	const unsigned char* activeVaultSalt = vaultMetaData[0].vaultKeyNonce;
	std::string activeVaultName = vaultMetaData[0].vaultName;
	std::string metaCommand = args.getArg("-v");
	if (metaCommand == "list") {
		// Check that vaultMetaData is not empty
		if (vaultMetaData.empty()) {
			std::cout << "Error: vaultMetaData is empty" << std::endl;
		}
		else {
			// Skip the first one when listing because first one is a duplicate for the active vault
			for (int i = 1; i < vaultMetaData.size(); i++) {
				std::cout << vaultMetaData[i].vaultName << std::endl;
			}
		}

		return;
	}

	std::string vaultKey = args.getArg("-k");
	if (vaultKey == "") {
		std::cout << "Error: Vault key must be provided to make changes to a vault." << std::endl;
		return;
	}

	if (metaCommand == "add") {
		// Create a new vault:
		std::string newVaultName = args.getArg("-n");

		// Error if the vault already exists:
		for (int i = 1; i < vaultMetaData.size(); i++) {
			if (vaultMetaData[i].vaultName == newVaultName) {
				std::cout << "Error: A vault with the given name already exists." << std::endl;
				return;
			}
		}

		Vault newVault(newVaultName, vaultKey, true);
		newVault.writeVault();

		// Generate pw hash and salt for the new vault

		// Compute random salt:
		unsigned char salt[SKEY_LENGTH];
		Utils::genRand(salt, SKEY_LENGTH);

		// Generate pw hash
		unsigned char newVaultKeyHash[SKEY_LENGTH];
		unsigned char skeyHash[SKEY_LENGTH];
		unsigned char concatBuffer[SKEY_LENGTH * 2];
		Utils::sha256(skeyHash, (unsigned char *)vaultKey.c_str(), vaultKey.size());
		Utils::concatArr(skeyHash, salt, SKEY_LENGTH, SKEY_LENGTH, concatBuffer);
		Utils::sha256(newVaultKeyHash, concatBuffer, SKEY_LENGTH * 2);
		// unsigned char newVaultKeyHash = sha256(sha256(vaultKey).c_str() + salt.c_str());

		// Switch active vault to the new vault if there is no active vault (activeVaultName == ""):
		// Since the meta file is empty, the new vault metadata need to be written twice because the top 
		//		 spot in the metadata is reserved for a duplicate used to indicate which vault is active
		if (activeVaultName == "") {
			std::ofstream fileStream("meta/meta");
			fileStream.write(newVaultKeyHash, SKEY_LENGTH);
			fileStream.write(salt, SKEY_LENGTH);
			fileStream.write(newVaultName.c_str(), newVaultName.size());
			fileStream.write("\n", 1);
			fileStream.write(newVaultKeyHash, SKEY_LENGTH);
			fileStream.write(salt, SKEY_LENGTH);
			fileStream.write(newVaultName.c_str(), newVaultName.size());
			fileStream.write("\n", 1);
			fileStream.close();
		} else {
			// If there is active vault, append new vault name to end of meta/meta only once
			std::ofstream fileStream("meta/meta");
			fileStream.write(newVaultKeyHash, SKEY_LENGTH);
			fileStream.write(salt, SKEY_LENGTH);
			fileStream.write(newVaultName.c_str(), newVaultName.size());
			fileStream.write("\n", 1);
		}
	} else if (metaCommand == "update") {
		// Error if there is no active vault:
		if (activeVaultName == "") {
			std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
			return;
		}

		// TODO: Verify that vaultKey is correct and report error and exit if not
		std::string newVaultKey = args.getArg("-knew");
		if (newVaultKey == "") {
			std::cout << "Error: New vault key must be provided to update vault key" << std::endl;
			return;
		}

		unsigned char providedKeyHash[SKEY_LENGTH];
		unsigned char oldskeyHash[SKEY_LENGTH];
		unsigned char concatBuffer[SKEY_LENGTH * 2];
		Utils::sha256(oldskeyHash, (unsigned char *)vaultKey.c_str(), vaultKey.size());
		Utils::concatArr(oldskeyHash, salt, SKEY_LENGTH, SKEY_LENGTH, concatBuffer);
		Utils::sha256(providedKeyHash, concatBuffer, SKEY_LENGTH * 2);

		// std::string providedKeyHash = sha256(sha256(vaultKey) + activeVaultSalt)
		if (providedKeyHash != activeVaultHash) {
			std::cout << "Error: Provided vaultKey is incorrect" << std::endl;
			return;
		}
		
		Vault activeVault(vaultName, vaultKey);
		activeVault.updateKey(newVaultKey);
		activeVault.writeVault();
	} else if (metaCommand == "switch") {
		// Error if there is no active vault:
		if (activeVaultName == "") {
			std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
			return;
		}

		std::string vaultToSwitchToName = args.getArg("-n");
		// TODO: In the meta/meta file, swap the active vault's name with the name of the vault to switch to:

	} else if (metaCommand == "delete") {
		// Error if there is no active vault:
		if (activeVaultName == "") {
			std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
			return;
		}

		// TODO: Verify that vaultKey is correct and report error and exit if not

		std::string vaultToDeleteName = args.getArg("-n");
		// TODO: Remove the vault to delete's name from the meta/meta file

		// Remove the vault file in the 'vaults' directory:
		std::remove("vaults/" + vaultToDeleteName);
	} else {
		std::cout << "Error: Invalid vault command\n"
			<< "Valid commands are: add, update, switch, delete, list" << std::endl;
		return;
	}
}

/**
	Process a command that pertains to some account (or accounts) in the currently active vault
*/
void processAccountCommand(const CommandLineParser& args, const std::vector<VaultInfo> &vaultMetaData)
{
	if (vaultMetaData.empty()) {
		std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
		return;
	}

	std::string vaultKey = arg.getArg("-k");
	if (vaultKey == "") {
		std::cout << "Error: Vault key must be provided to access the active vault's accounts." << std::endl;
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
		std::cout << activeVault.getAccount(accountName).getUsername() << std::endl;
	} else if (args.containsArg("-pw")) {
		std::cout << activeVault.getAccount(accountName).getPassword() << std::endl;
	} else if (args.containsArg("-note")) {
		std::cout << activeVault.getAccount(accountName).getNote() << std::endl;
	} else {
		const Account account = activeVault.getAccount(accountName);
		std::cout << "un=" << account.getUsername() << '\n'
			<< "pw=" << account.getPassword() << '\n'
			<< "note=" << account.getNote() << std::endl;
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
