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
void processVaultCommand(const CommandLineParser& args, std::vector<VaultInfo> &vaultMetaData);
void processAccountCommand(const CommandLineParser& args, const std::vector<VaultInfo> &vaultMetaData);
void processAccountPrintCommand(const CommandLineParser& args, Vault &activeVault);
void processAccountClipCommand(const CommandLineParser& args, Vault &activeVault);
void processAccountUpdateCommand(const CommandLineParser& args, Vault &activeVault);
void processAccountAddCommand(const CommandLineParser& args, Vault &activeVault);

int main(int argc, char *argv[]) {
	Utils::debugDisable();
	Utils::debugPrint(std::cout, "Entered main\n");

	std::vector<VaultInfo> vaultMetaData;
	initialize(vaultMetaData);

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

		// // Assume that there never exists a meta/meta file containing no data or with 0 length:
		// return vaultMetaData[0].vaultName; // the active vault is first
	}
}

/**
	Process a command that pertains to an entire vault.
*/
void processVaultCommand(const CommandLineParser& args, std::vector<VaultInfo> &vaultMetaData) {	
	Utils::debugPrint(std::cout, "Entered processVaultCommand\n");

	std::string metaCommand = args.getArg("-v");
	if (metaCommand == "list") {
		// Check that vaultMetaData is not empty
		if (vaultMetaData.empty()) {
			std::cout << "Error: vaultMetaData is empty" << std::endl;
		} else {
			std::cout << "Current active vault: " + vaultMetaData[0].vaultName + "."<< std::endl;
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
		VaultInfo newVaultInfo;
		newVaultInfo.vaultName = args.getArg("-n");
		if (newVaultInfo.vaultName == "") {
			std::cout << "Error: You must provide an account name using the -n option." << std::endl;
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
		std::string newVaultKey = args.getArg("-knew");
		if (newVaultKey == "") {
			std::cout << "Error: New vault key must be provided to update vault key" << std::endl;
			return;
		}

		const unsigned char* activeVaultHash = vaultMetaData[0].vaultSkeyHash;
		unsigned char* activeVaultSalt = vaultMetaData[0].vaultSkeySalt;
		std::string activeVaultName = vaultMetaData[0].vaultName;
		
		// Utils::verifyKey(vaultKey, vaultMetaData[i].vaultSkeySalt, 
		// 					vaultMetaData[i].vaultSkeyHash, SKEY_LENGTH))

		// Verify that vaultKey is correct and report error and exit if not:
		if (!Utils::verifyKey(vaultKey, activeVaultSalt, activeVaultHash, SKEY_LENGTH)) {
			std::cout << "Error: Provided vault key is incorrect" << std::endl;
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
		std::string vaultToSwitchToName = args.getArg("-n");
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
				if (!Utils::verifyKey(vaultKey, vaultMetaData[i].vaultSkeySalt, 
					vaultMetaData[i].vaultSkeyHash, SKEY_LENGTH)) {
					std::cout << "Error: Provided vault key is incorrect." << std::endl;
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

		std::string vaultToDeleteName = args.getArg("-n");
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
				// Utils::debugPrint(std::cout, vaultMetaData[i].vaultName + " saved vault name \n");
				// Utils::debugPrint(std::cout, std::to_string(vaultMetaData[i].vaultSkeyHash[0]) + " saved vault hash \n");
				// Utils::debugPrint(std::cout, std::to_string(vaultMetaData[i].vaultSkeySalt[0]) + " saved vault salt \n");
				// now fixed: verifyKey inconsistency was caused by using key.length() as keylen while it should be SKEY_LENGTH
				if (!Utils::verifyKey(vaultKey, vaultMetaData[i].vaultSkeySalt, 
					vaultMetaData[i].vaultSkeyHash, SKEY_LENGTH)) {
					std::cout << "Error: Provided vaultKey is incorrect" << std::endl;
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

	} else {
		std::cout << "Error: Invalid vault command\n"
			<< "Valid commands are: add, update, switch, delete, list" << std::endl;
		return;
	}
}

/**
	Processes a command that pertains to some account (or accounts) in the currently active vault.
*/
void processAccountCommand(const CommandLineParser& args, const std::vector<VaultInfo> &vaultMetaData) {
	Utils::debugPrint(std::cout, "Entered processAccountCommand\n");

	if (vaultMetaData.empty()) {
		std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
		return;
	}

	std::string vaultKey = args.getArg("-k");
	if (vaultKey == "") {
		std::cout << "Error: Vault key must be provided to access the active vault's accounts." << std::endl;
		return;
	}

	// Verify that vaultKey is correct and report error and exit if not:
	if (!Utils::verifyKey(vaultKey, vaultMetaData[0].vaultSkeySalt, vaultMetaData[0].vaultSkeyHash, SKEY_LENGTH)) {
		std::cout << "Error: Provided vault key is incorrect" << std::endl;
		return;
	}

	// Attempt to load and decrypt vault:
	Vault activeVault(vaultMetaData[0].vaultName, vaultKey);
	if (args.containsArg("-p")) {
		processAccountPrintCommand(args, activeVault);
	} else if (args.containsArg("-c")) {
		processAccountClipCommand(args, activeVault);
	} else if (args.containsArg("-u")) {
		processAccountUpdateCommand(args, activeVault);
	} else if (args.containsArg("-a")) {
		processAccountAddCommand(args, activeVault);
	} else {
		std::cout << "Error: Invalid account command. Valid command options are: -p, -c, -u, -a" << std::endl;
		return;
	}
}

/**
	Processes a print command. Assumes the active vault has successfully been decrypted.
*/
void processAccountPrintCommand(const CommandLineParser& args, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountPrintCommand\n");

	if (args.containsArg("-l")) {
		activeVault.printTags(std::cout);
		return;
	}

	std::string accountName = args.getArg("-n");
	if (accountName == "") {
		std::cout << "Error: You must provide an account name using the -n option." << std::endl;
		return;
	}
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
void processAccountClipCommand(const CommandLineParser& args, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountClipCommand\n");

	std::string accountName = args.getArg("-n");
	if (accountName == "") {
		std::cout << "Error: You must provide an account name using the -n option." << std::endl;
		return;
	}

	// TODO: How to get xclip to not add a newline to end of copied string??
	if (args.containsArg("-un")) {
		system(("echo " + activeVault.getAccount(accountName).getUsername() + " | xclip -selection c").c_str());
	} else if (args.containsArg("-pw")) {
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
void processAccountUpdateCommand(const CommandLineParser& args, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountUpdateCommand\n");

	std::string accountName = args.getArg("-n");
	if (accountName == "") {
		std::cout << "Error: You must provide an account name using the -n option." << std::endl;
		return;
	}

	if (!activeVault.exists(accountName)) {
		std::cout << "Error: The specified account does not exist. You may create an account using the -a option." << std::endl;
		return;
	}

	if (args.containsArg("-un")) {
		std::string username = args.getArg("-un");
		// Update the username of the given account
		Account *account = &activeVault.getAccount(accountName);
		account->setUsername(username);
	} else if (args.containsArg("-pw")) {
		std::string password = args.getArg("-pw");
		// Update the password of the given account
		Account *account = &activeVault.getAccount(accountName);
		account->setPassword(password);
	} else if (args.containsArg("-note")) {
		std::string note = args.getArg("-note");
		// Update the note of the given account
		Account *account = &activeVault.getAccount(accountName);
		account->setNote(note);
	} else if (args.containsArg("-f")) {
		std::string filePath = args.getArg("-f");
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
void processAccountAddCommand(const CommandLineParser& args, Vault &activeVault) {
	Utils::debugPrint(std::cout, "Entered processAccountAddCommand\n");

	std::string accountName = args.getArg("-n");
	if (accountName == "") {
		std::cout << "Error: You must provide an account name using the -n option." << std::endl;
		return;
	}

	if (activeVault.exists(accountName)) {
		std::cout << "Error: There already exists an account with the specified name." << std::endl;
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

	activeVault.writeVault();
}
