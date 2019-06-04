#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <vector>
#include <cstring>

#include "CommandLineParser.h"
#include "Vault.h"
#include "Utils.h"

struct VaultInfo {
	unsigned char vaultSkeyHash[SKEY_LENGTH]; // vaultSkeyHash = sha256(sha256(skey) + vaultSkeySalt)
	unsigned char vaultSkeySalt[SKEY_LENGTH];
	std::string vaultName;
};

void initialize(std::vector<VaultInfo> &vaultMetaData);
void readVaultMetaData(std::vector<VaultInfo> &vaultMetaData);
void writeVaultMetaData(std::vector<VaultInfo> &vaultMetaData);
std::string getVaultKey(const CommandLineParser& commandOpts);
std::string getAccountName(const CommandLineParser& commandOpts);
bool validateKey(std::string key, const unsigned char *salt, const unsigned char *hash);

void processVaultCommand(const CommandLineParser& commandOpts, std::vector<VaultInfo> &vaultMetaData);
void processAccountCommand(const CommandLineParser& commandOpts, const std::vector<VaultInfo> &vaultMetaData);
void processAccountPrintCommand(const CommandLineParser& commandOpts, Vault &activeVault);
void processAccountClipCommand(const CommandLineParser& commandOpts, Vault &activeVault);
void processAccountUpdateCommand(const CommandLineParser& commandOpts, Vault &activeVault);
void processAccountAddCommand(const CommandLineParser& commandOpts, Vault &activeVault);

int main(int argc, char *argv[]) {
	Utils::debugDisable();
	Utils::debugPrint(std::cout, "Entered main\n");

	std::vector<VaultInfo> vaultMetaData;
	initialize(vaultMetaData);

	CommandLineParser commandOpts(argc, argv);
	if (commandOpts.containsOpt(CommandLineOptions::VAULT_OPTION)) {
		// This is a vault command
		processVaultCommand(commandOpts, vaultMetaData);
	} else {
		// This is a command that pertains to some account (or accounts) in the currently active vault
		processAccountCommand(commandOpts, vaultMetaData);
	}
}

/**
	Reads all vault metadata from the meta/meta file and creates the meta and vaults
	directories if they do not yet exist. Stores all vault metadata in the 'vaultMetaData'
	vector.
*/
void initialize(std::vector<VaultInfo> &vaultMetaData) {
	Utils::debugPrint(std::cout, "Entered initialize\n");

	struct stat info;
	if (((stat("meta", &info) != 0)) ||
		((stat("vaults", &info) != 0))) {
		// meta or vaults directories do not exist:
		
		// Create empty meta and vaults directories:
		system("mkdir meta/");
		system("mkdir vaults/");
		//system("touch meta/meta");
		//system("touch vaults/default");
	} else if ((stat("meta/meta", &info) != 0)) {
		// meta/meta file does not exist:
	} else {
		// Read meta/meta file:
		readVaultMetaData(vaultMetaData);
	}
}

/**
	Read all of the vault metadata into the in-memory vaultMetaData vector.
	The metadata in the meta file is of the following format:

	uint32: n = number of vaults
	
	uint32: s = size of the active vault's name (in bytes)
	s bytes: name = active vault's name
	32 bytes: active vault's hash = sha256(sha256(vaultKey) || salt)
	32 bytes: active vault's salt = some random 32-byte value

	<repeated n times>:
		uint32: s = size of the vault's name (in bytes)
		s bytes: name = vault's name
		32 bytes: hash = sha256(sha256(vaultKey) || salt)
		32 bytes: salt = some random 32-byte value
*/
void readVaultMetaData(std::vector<VaultInfo> &vaultMetaData) {
	Utils::debugPrint(std::cout, "Entered readVaultMetaData\n");

	std::ifstream fileStream("meta/meta");

	uint32_t numVaults;
	fileStream.read((char *)&numVaults, sizeof(numVaults));

	VaultInfo vaultInfo;
	uint32_t vaultNameSize;
	for (int i = 0; i < numVaults; ++i) {
		fileStream.read((char *)&vaultNameSize, sizeof(vaultNameSize)); // read vault name's size
		vaultInfo.vaultName.resize(vaultNameSize); // reserve vaultNameSize bytes
		fileStream.read((char *)&vaultInfo.vaultName[0], vaultNameSize); // read vaultName from file
		fileStream.read((char *)vaultInfo.vaultSkeyHash, SKEY_LENGTH);
		fileStream.read((char *)vaultInfo.vaultSkeySalt, SKEY_LENGTH);
		vaultMetaData.push_back(vaultInfo);
	}
	fileStream.close();
}

/**
	Write all of the vaultMetaData vector to the vault metadata file.
	The vault metadata is written to the file in the following format:

	uint32: n = number of vaults
	
	uint32: s = size of the active vault's name (in bytes)
	s bytes: name = active vault's name
	32 bytes: active vault's hash = sha256(sha256(vaultKey) || salt)
	32 bytes: active vault's salt = some random 32-byte value

	<repeated n times>:
		uint32: s = size of the vault's name (in bytes)
		s bytes: name = vault's name
		32 bytes: hash = sha256(sha256(vaultKey) || salt)
		32 bytes: salt = some random 32-byte value
*/
void writeVaultMetaData(std::vector<VaultInfo> &vaultMetaData) {
	Utils::debugPrint(std::cout, "Entered writeVaultMetaData\n");

	std::ofstream fileStream("meta/meta");

	uint32_t numVaults = vaultMetaData.size();
	fileStream.write((char *)&numVaults, sizeof(numVaults));

	VaultInfo vaultInfo;
	uint32_t vaultNameSize;
	for (int i = 0; i < numVaults; ++i) {
		vaultInfo = vaultMetaData[i];
		vaultNameSize = vaultInfo.vaultName.size();
		fileStream.write((char *)&vaultNameSize, sizeof(vaultNameSize)); // write vault name's size
		fileStream.write((char *)vaultInfo.vaultName.c_str(), vaultNameSize); // write vaultName to file
		fileStream.write((char *)vaultInfo.vaultSkeyHash, SKEY_LENGTH);
		fileStream.write((char *)vaultInfo.vaultSkeySalt, SKEY_LENGTH);
	}
	fileStream.close();
}

/**
	Attempts to retrieve the KEY_OPTION parameter from the command options and
	reports a generic error if the option does not exist.
*/
std::string getVaultKey(const CommandLineParser& commandOpts) {
	std::string vaultKey = commandOpts.getOpt(CommandLineOptions::KEY_OPTION);
	if (vaultKey == "") {
		std::cout << "Error: You must provide a vault key using the -k option." << std::endl;
	}
	return vaultKey;
}

/**
	Attempts to retrieve the NAME_OPTION parameter from the command options and
	reports a generic error if the option does not exist.
*/
std::string getAccountName(const CommandLineParser& commandOpts) {
	std::string name = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
	if (name == "") {
		std::cout << "Error: You must provide an account name using the -n option." << std::endl;
	}
	return name;
}

/**
	Returns true if the provided vaultKey verifies using the given salt and salted hash values.
	Returns False and reports an error otherwise.
*/
bool validateKey(std::string key, const unsigned char *salt, const unsigned char *hash) {
	if (!Utils::verifyKey(key, salt, hash, SKEY_LENGTH)) {
		std::cout << "Error: The provided vault key is incorrect." << std::endl;
		return false;
	}
	return true;
}

/**
	Process a command that pertains to an entire vault.
*/
void processVaultCommand(const CommandLineParser& commandOpts, std::vector<VaultInfo> &vaultMetaData) {	
	Utils::debugPrint(std::cout, "Entered processVaultCommand\n");

	std::string metaCommand = commandOpts.getOpt(CommandLineOptions::VAULT_OPTION);
	if (metaCommand == "list" && !commandOpts.containsOpt(CommandLineOptions::KEY_OPTION)) {
		// Check that vaultMetaData is not empty
		if (vaultMetaData.empty()) {
			std::cout << "Error: vaultMetaData is empty" << std::endl;
		} else {
			std::cout << "Current active vault: " + vaultMetaData[0].vaultName + "." << std::endl;
			// Skip the first one when listing because first one is a duplicate for the active vault
			for (int i = 1; i < vaultMetaData.size(); i++) {
				std::cout << vaultMetaData[i].vaultName << std::endl;
			}
		}

		return;
	}

	std::string vaultKey = getVaultKey(commandOpts);

	if (metaCommand == "add") {
		// Create a new vault:
		VaultInfo newVaultInfo;
		newVaultInfo.vaultName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
		if (newVaultInfo.vaultName == "") {
			std::cout << "Error: You must provide a vault name using the -n option." << std::endl;
			return;
		}
	
		Utils::debugPrint(std::cout, newVaultInfo.vaultName + " newvaultname \n");
		// Error if the vault already exists:
		for (int i = 1; i < vaultMetaData.size(); i++) {
			// Utils::debugPrint(std::cout, std::to_string(i) + " index \n");
			// Utils::debugPrint(std::cout, vaultMetaData[i].vaultName + " name \n");
			// Utils::debugPrint(std::cout, std::to_string(vaultMetaData[i].vaultSkeyHash[0]) + " hash \n");
			// Utils::debugPrint(std::cout, std::to_string(vaultMetaData[i].vaultSkeySalt[0]) + " salt \n");
			if (vaultMetaData[i].vaultName == newVaultInfo.vaultName) {
				std::cout << "Error: A vault with the given name already exists." << std::endl;
				return;
			}
		}
		
		Vault newVault(newVaultInfo.vaultName, vaultKey, true);
		newVault.writeVault();

		// Generate pw hash and salt for the new vault

		// Compute random salt:
		Utils::genRand(newVaultInfo.vaultSkeySalt, SKEY_LENGTH);

		// Generate pw hash
		unsigned char newVaultSkey[SKEY_LENGTH];
		unsigned char concatBuffer[SKEY_LENGTH * 2];
		Utils::sha256(newVaultSkey, (unsigned char *)vaultKey.c_str(), vaultKey.size());
		Utils::concatArr(newVaultSkey, newVaultInfo.vaultSkeySalt, SKEY_LENGTH, SKEY_LENGTH, concatBuffer);
		Utils::sha256(newVaultInfo.vaultSkeyHash, concatBuffer, SKEY_LENGTH * 2);

		// Add new vault metadata to vector of vault metadatam:
		vaultMetaData.push_back(newVaultInfo);

		// Switch active vault to the new vault if there is no active vault (activeVaultName == ""):
		// Since the meta file is empty, the new vault metadata need to be written twice because the top 
		//		 spot in the metadata is reserved for a duplicate used to indicate which vault is active
		if (vaultMetaData.size() == 1) {
			vaultMetaData.push_back(newVaultInfo);
		}
		Utils::debugPrint(std::cout, newVaultInfo.vaultName + " new vault name \n");
		Utils::debugPrint(std::cout, "creating new vault using key = " + vaultKey + "\n");
		Utils::debugPrint(std::cout, std::to_string(newVaultInfo.vaultSkeyHash[0]) + " new vault hash \n");
		Utils::debugPrint(std::cout, std::to_string(newVaultInfo.vaultSkeySalt[0]) + " new vault salt \n");

		// Write the updated vault metadata to disk:
		writeVaultMetaData(vaultMetaData);
	} else if (metaCommand == "update") {
		// Error if there is no active vault:
		if (vaultMetaData.empty()) {
			std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
			return;
		}

		// Verify that vaultKey is correct and report error and exit if not
		std::string newVaultKey = commandOpts.getOpt(CommandLineOptions::NEWKEY_OPTION);
		if (newVaultKey == "") {
			std::cout << "Error: New vault key must be provided to update vault key" << std::endl;
			return;
		}

		const unsigned char* activeVaultHash = vaultMetaData[0].vaultSkeyHash;
		unsigned char* activeVaultSalt = vaultMetaData[0].vaultSkeySalt;
		std::string activeVaultName = vaultMetaData[0].vaultName;

		// Verify that vaultKey is correct and report error and exit if not:
		if (!validateKey(vaultKey, activeVaultSalt, activeVaultHash)) {
			return;
		}

		// Compute and store new random salt:
		unsigned char newSalt[SKEY_LENGTH];
		Utils::genRand(newSalt, SKEY_LENGTH);
		std::memcpy(activeVaultSalt, newSalt, SKEY_LENGTH);
		
		// Re-encrypt active Vault with new key:
		Vault activeVault(activeVaultName, vaultKey);
		activeVault.updateKey(newVaultKey);
		activeVault.writeVault();

		// fixed: added changes to metadata after updating vault key
		VaultInfo UpadatedVaultInfo;
		UpadatedVaultInfo.vaultName = activeVaultName;
		std::memcpy(UpadatedVaultInfo.vaultSkeySalt, newSalt, SKEY_LENGTH);

		// calcuate new hash for updated metadata and fill UpadatedVaultInfo.vaultSkeyHash
		unsigned char concatBuffer[SKEY_LENGTH * 2];
		Utils::sha256(UpadatedVaultInfo.vaultSkeyHash, (unsigned char *)newVaultKey.c_str(), newVaultKey.size());
		Utils::concatArr(UpadatedVaultInfo.vaultSkeyHash, newSalt, SKEY_LENGTH, SKEY_LENGTH, concatBuffer);
		Utils::sha256(UpadatedVaultInfo.vaultSkeyHash, concatBuffer, SKEY_LENGTH * 2);

		// Search in vaultMetaData for VaultInfo to update
		for (int i = 1; i < vaultMetaData.size(); i++) { 
			if (vaultMetaData[i].vaultName == activeVaultName) {
				// overwrite the metadata of the corresponding vault, 
				// 	along with the active vault duplicate
				vaultMetaData[0] = UpadatedVaultInfo;
				vaultMetaData[i] = UpadatedVaultInfo;
				break;
			}
		}

		// Update vault metadata file:
		writeVaultMetaData(vaultMetaData);
		std::cout << "Successfully updated key for vault " + activeVaultName << std::endl;
	} else if (metaCommand == "switch") {
		std::string vaultToSwitchToName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
		if (vaultToSwitchToName == "") {
			std::cout << "Error: Must provide the name of the vault to which you wish to switch to." << std::endl;
		}

		if (vaultMetaData.empty()) {
			// Error if there is no active vault:
			std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
			return;
		} else if (vaultMetaData[0].vaultName == vaultToSwitchToName) {
			std::cout << "Error: " + vaultToSwitchToName + " is already the active vault." << std::endl;
			return;
		}

		for (int i = 1; i < vaultMetaData.size(); ++i) {
			// Find the vault to switch to
			if (vaultMetaData[i].vaultName == vaultToSwitchToName) {
				// Validate key
				if (!validateKey(vaultKey, vaultMetaData[i].vaultSkeySalt, vaultMetaData[i].vaultSkeyHash)) {
					return;
				}
				// copy its metadata to the 0th position in the VaultInfo vector:
				vaultMetaData[0] = vaultMetaData[i];

				// Write updates to the VaultInfo vector to disk:
				writeVaultMetaData(vaultMetaData);
				std::cout << "Switched to vault " + vaultMetaData[0].vaultName + "."<< std::endl;
				return;
			}
		}

		// No match found, no vault with name equal to vaultToSwitchToName exist in meta file
		std::cout << "Error: No vault with the name of \"" + vaultToSwitchToName + "\" exist" << std::endl;
	} else if (metaCommand == "delete") {
		// Error if there is no active vault:
		if (vaultMetaData.empty()) {
			std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
			return;
		}

		const unsigned char* activeVaultHash = vaultMetaData[0].vaultSkeyHash;
		const unsigned char* activeVaultSalt = vaultMetaData[0].vaultSkeySalt;
		std::string activeVaultName = vaultMetaData[0].vaultName;

		std::string vaultToDeleteName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
		if (vaultToDeleteName == "") {
			std::cout << "Error: You must specify which vault to delete." << std::endl;
			return;
		} else if (vaultToDeleteName == activeVaultName) {
			std::cout << "Error: You cannot delete a vault that is currently active." << std::endl;
			return;
		}

		std::string filePathToRemove = "vaults/" + vaultToDeleteName;

		// Search in vaultMetaData for VaultInfo to delete
		for (int i = 1; i < vaultMetaData.size(); i++) { 
			if (vaultMetaData[i].vaultName == vaultToDeleteName) {
				// Verify that vaultKey is correct and report error and exit if not
				if (!validateKey(vaultKey, vaultMetaData[i].vaultSkeySalt, vaultMetaData[i].vaultSkeyHash)) {
					return;
				} 
				// Remove the vault to delete's name from the meta/meta file
				vaultMetaData.erase(vaultMetaData.begin() + i);
				// Update vault metadata file:
				writeVaultMetaData(vaultMetaData);
				// Remove the vault file in the 'vaults' directory:
				std::remove(filePathToRemove.c_str());
				std::cout << vaultToDeleteName + " has been deleted."<< std::endl;
				return;
			}
		}
		// No match found, no vault with name equal to vaultToDeleteName exist in meta file
		std::cout << "Error: No vault with the name of \"" + vaultToDeleteName + "\" exist" << std::endl;
		return;

	} else if (metaCommand == "list") {
		if (!validateKey(vaultKey, vaultMetaData[0].vaultSkeySalt, vaultMetaData[0].vaultSkeyHash)) {
			return;
		}
		Vault activeVault(vaultMetaData[0].vaultName, vaultKey);

		if (commandOpts.containsOpt(CommandLineOptions::INFO_OPTION)) {
			activeVault.printInfo(std::cout);
		} else {
			activeVault.printTags(std::cout);
		}
	} else {
		std::cout << "Error: Invalid vault command\n"
			<< "Valid commands are: add, update, switch, delete, list" << std::endl;
		return;
	}
}

/**
	Processes a command that pertains to some account in the currently active vault.
*/
void processAccountCommand(const CommandLineParser& commandOpts, const std::vector<VaultInfo> &vaultMetaData) {
	Utils::debugPrint(std::cout, "Entered processAccountCommand\n");

	if (vaultMetaData.empty()) {
		std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
		return;
	}

	std::string vaultKey = getVaultKey(commandOpts);

	// Verify that vaultKey is correct and report error and exit if not:
	if (!validateKey(vaultKey, vaultMetaData[0].vaultSkeySalt, vaultMetaData[0].vaultSkeyHash)) {
		return;
	}

	// Attempt to load and decrypt vault:
	Vault activeVault(vaultMetaData[0].vaultName, vaultKey);
	if (commandOpts.containsOpt(CommandLineOptions::PRINT_OPTION)) {
		processAccountPrintCommand(commandOpts, activeVault);
	} else if (commandOpts.containsOpt(CommandLineOptions::CLIP_OPTION)) {
		processAccountClipCommand(commandOpts, activeVault);
	} else if (commandOpts.containsOpt(CommandLineOptions::UPDATE_OPTION)) {
		processAccountUpdateCommand(commandOpts, activeVault);
	} else if (commandOpts.containsOpt(CommandLineOptions::ADD_OPTION)) {
		processAccountAddCommand(commandOpts, activeVault);
	} else {
		std::cout << "Error: Invalid account command. Valid command options are: -p, -c, -u, -a" << std::endl;
		return;
	}
}

/**
	Processes a print command. Assumes the active vault has successfully been decrypted.
*/
void processAccountPrintCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountPrintCommand\n");

	std::string accountName = getAccountName(commandOpts);
	if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION)) {
		std::cout << activeVault.getAccount(accountName).getUsername() << std::endl;
	} else if (commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
		std::cout << activeVault.getAccount(accountName).getPassword() << std::endl;
	} else if (commandOpts.containsOpt(CommandLineOptions::NOTE_OPTION)) {
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
void processAccountClipCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountClipCommand\n");

	std::string accountName = getAccountName(commandOpts);

	// TODO: How to get xclip to not add a newline to end of copied string??
	if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION)) {
		system(("echo " + activeVault.getAccount(accountName).getUsername() + " | xclip -selection c").c_str());
	} else if (commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
		system(("echo " + activeVault.getAccount(accountName).getPassword() + " | xclip -selection c").c_str());
	} else {
		std::cout << "Error: Invalid clip option. Valid options are -un and -pw." << std::endl;
	}
	// https://stackoverflow.com/questions/6436257/how-do-you-copy-paste-from-the-clipboard-in-c
	// https://stackoverflow.com/questions/40436045/in-qt-how-can-i-register-a-qstring-to-my-systems-clipboard-both-quoted-and-no/40437290#40437290

	// https://linoxide.com/linux-how-to/copy-paste-commands-output-xclip-linux/

	std::cout << "Copied requested content to clipboard." << std::endl;
	return;
}

/**
	Processes an update command. Assumes the active vault has successfully been decrypted.
*/
void processAccountUpdateCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountUpdateCommand\n");

	std::string accountName = getAccountName(commandOpts);

	if (!activeVault.exists(accountName)) {
		std::cout << "Error: The specified account does not exist. You may create an account using the -a option." << std::endl;
		return;
	}

	if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION)) {
		std::string username = commandOpts.getOpt(CommandLineOptions::USERNAME_OPTION);
		// Update the username of the given account
		Account *account = &activeVault.getAccount(accountName);
		account->setUsername(username);
	} else if (commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
		std::string password = commandOpts.getOpt(CommandLineOptions::PASSWORD_OPTION);
		// Update the password of the given account
		Account *account = &activeVault.getAccount(accountName);
		account->setPassword(password);
	} else if (commandOpts.containsOpt(CommandLineOptions::NOTE_OPTION)) {
		std::string note = commandOpts.getOpt(CommandLineOptions::NOTE_OPTION);
		// Update the note of the given account
		Account *account = &activeVault.getAccount(accountName);
		account->setNote(note);
	} else if (commandOpts.containsOpt(CommandLineOptions::FILE_OPTION)) {
		std::string filePath = commandOpts.getOpt(CommandLineOptions::FILE_OPTION);
		// Update all details of the given account
		Account account(accountName, filePath);
		activeVault.addAccount(account);
	} else {
		std::cout << "Error: Invalid account update option. Valid options are -un, -pw, -note, and -f." << std::endl;
		return;
	}

	activeVault.writeVault();
}

/**
	Processes an add command. Assumes the active vault has successfully been decrypted.
*/
void processAccountAddCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountAddCommand\n");

	std::string accountName = getAccountName(commandOpts);

	if (activeVault.exists(accountName)) {
		std::cout << "Error: There already exists an account with the specified name." << std::endl;
		return;
	}

	if (commandOpts.containsOpt(CommandLineOptions::FILE_OPTION)) {
		std::string filePath = commandOpts.getOpt(CommandLineOptions::FILE_OPTION);
		// Read the new account from the specified file
		Account account(accountName, filePath);
		activeVault.addAccount(account);
	} else if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION) && commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
		std::string username = commandOpts.getOpt(CommandLineOptions::USERNAME_OPTION);
		std::string password = commandOpts.getOpt(CommandLineOptions::PASSWORD_OPTION);
		// Create a new account with the given username and password
		Account account(accountName, username, password);
		activeVault.addAccount(account);
	} else {
		// Create a new account with no details
		Account account(accountName);
		activeVault.addAccount(account);
	}

	activeVault.writeVault();
}
