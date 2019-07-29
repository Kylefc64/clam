#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

#include "CommandLineParser.h"
#include "Vault.h"
#include "Utils.h"
#include "VaultManager.h"

const std::string getProgramName(char *argv[]);
const std::string getUserHomeDir();
void initDataDirs(const std::string &programDataDir, const std::string &vaultDir);
const std::string getVaultKey(const CommandLineParser& commandOpts);
const std::string getAccountName(const CommandLineParser& commandOpts, CommandLineOptions nameOpt);

void handleInvalidCommand(const std::string &errorDetails);
void processVaultCommand(const CommandLineParser& commandOpts, VaultManager &vaultManager);
void processHelpCommand();
void processAccountCommand(const CommandLineParser& commandOpts, VaultManager &vaultManager);
void processAccountPrintCommand(const CommandLineParser& commandOpts, Vault &activeVault);
void processAccountClipCommand(const CommandLineParser& commandOpts, Vault &activeVault);
void processAccountUpdateCommand(const CommandLineParser& commandOpts, Vault &activeVault);
void processAccountAddCommand(const CommandLineParser& commandOpts, Vault &activeVault);

int main(int argc, char *argv[]) {
    Utils::debugDisable();
    Utils::debugPrint(std::cout, "Entered main\n");

    const std::string programName = getProgramName(argv);
    const std::string userHomeDir = getUserHomeDir();
    const std::string programDataDir = std::string(userHomeDir) + "/." + programName + "/";
    const std::string metadataFilePath = programDataDir + "meta";
    const std::string vaultDir = programDataDir + "vaults/";

    initDataDirs(programDataDir, vaultDir);

    VaultManager vaultManager(metadataFilePath, vaultDir);

    CommandLineParser commandOpts(argc, argv);
    if (commandOpts.containsOpt(CommandLineOptions::VAULT_OPTION)) {
        // This is a vault command
        processVaultCommand(commandOpts, vaultManager);
    } else if (commandOpts.containsOpt(CommandLineOptions::HELP_OPTION)) {
        processHelpCommand();
    } else {
        // This is a command that pertains to some account (or accounts) in the currently active vault
        processAccountCommand(commandOpts, vaultManager);
    }
}

/**
    Parses and returns the program's name from the command line args to this program.
*/
const std::string getProgramName(char *argv[]) {
    std::string binPath(argv[0]);
    std::string progName = binPath;
    std::size_t pos = 0;
    while ((pos = binPath.find('/', pos)) != std::string::npos) {
        progName = binPath.substr(++pos, binPath.size() - 1);
    }

    return progName;
}

/**
    Gets and returns the path to this user's home directory.
*/
const std::string getUserHomeDir() {
    // Get user's home directory (https://stackoverflow.com/questions/2910377/get-home-directory-in-linux):
    const char *userHomeDir;
    if (NULL == (userHomeDir = getenv("HOME"))) {
        userHomeDir = getpwuid(getuid())->pw_dir;
    }

    return std::string(userHomeDir);
}

/**
    Creates the program data directory and vaults subdirectory if either one does not yet exist.
*/
void initDataDirs(const std::string &programDataDir, const std::string &vaultDir) {
    Utils::debugPrint(std::cout, "Entered initialize\n");

    struct stat info;
    if (((stat(programDataDir.c_str(), &info) != 0)) ||
        ((stat(vaultDir.c_str(), &info) != 0))) {
        // program data or vaults directories do not exist:
        
        // Create empty program data and vaults directories:
        system(("mkdir " + programDataDir).c_str());
        system(("mkdir " + vaultDir).c_str());
    }
}

/**
    Attempts to retrieve the KEY_OPTION parameter from the command options and
    reports a generic error if the option does not exist.
*/
const std::string getVaultKey(const CommandLineParser& commandOpts) {
    const std::string vaultKey = commandOpts.getOpt(CommandLineOptions::KEY_OPTION);
    if (vaultKey == "") {
        handleInvalidCommand("No vault key provided.");
    }
    return vaultKey;
}

/**
    Attempts to retrieve the name parameter from the command options and
    reports a generic error if the option does not exist.
*/
const std::string getAccountName(const CommandLineParser& commandOpts, CommandLineOptions nameOpt) {
    const std::string name = commandOpts.getOpt(nameOpt);
    if (name == "") {
        handleInvalidCommand("No account name provided.");
    }
    return name;
}

/**
    Called when a parse error is encountered. Calls the processHelpCommand
    function to inform user of proper command syntax and exits the program.
*/
void handleInvalidCommand(const std::string &errorDetails) {
    std::cout << "Error: " << errorDetails << std::endl;
    processHelpCommand();
    exit(EXIT_SUCCESS);
}

/**
    Process a command that pertains to an entire vault.
*/
void processVaultCommand(const CommandLineParser& commandOpts, VaultManager &vaultManager) {
    Utils::debugPrint(std::cout, "Entered processVaultCommand\n");

    const std::string addOption = "add";
    const std::string updateOption = "update";
    const std::string switchOption = "switch";
    const std::string deleteOption = "delete";
    const std::string listOption = "list";

    const std::string metaCommand = commandOpts.getOpt(CommandLineOptions::VAULT_OPTION);
    if (metaCommand == listOption && !commandOpts.containsOpt(CommandLineOptions::KEY_OPTION)) {
        vaultManager.listVaultNames();
        return;
    }

    const std::string vaultKey = getVaultKey(commandOpts);

    if (metaCommand == addOption) {
        // Create a new vault:
        const std::string newVaultName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
        if (newVaultName == "") {
            handleInvalidCommand("Name of vault to add not provided.");
        }
        vaultManager.addVault(newVaultName, vaultKey);
    } else if (metaCommand == updateOption) {
        // Verify that vaultKey is correct and report error and exit if not
        const std::string newVaultKey = commandOpts.getOpt(CommandLineOptions::NEWKEY_OPTION);
        if (newVaultKey == "") {
            handleInvalidCommand("New vault key not provided.");
        }
        vaultManager.updateActiveVaultKey(vaultKey, newVaultKey);
    } else if (metaCommand == switchOption) {
        const std::string vaultToSwitchToName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
        if (vaultToSwitchToName == "") {
            handleInvalidCommand("Name of vault to switch to not provided.");
        }
        vaultManager.switchActiveVault(vaultKey, vaultToSwitchToName);     
    } else if (metaCommand == deleteOption) {
        const std::string vaultToDeleteName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
        if (vaultToDeleteName == "") {
            handleInvalidCommand("Name of vault to delete not provided.");
        }
        vaultManager.deleteVault(vaultKey, vaultToDeleteName);

    } else if (metaCommand == listOption) {
        if (!VaultManager::validateKey(vaultKey, vaultManager.activeVaultInfo().vaultSkeySalt, vaultManager.activeVaultInfo().vaultSkeyHash)) {
            return;
        }
        Vault activeVault(vaultManager.getVaultDir(), vaultManager.activeVaultInfo().vaultName, vaultKey);

        if (commandOpts.containsOpt(CommandLineOptions::INFO_OPTION)) {
            activeVault.printInfo(std::cout);
        } else {
            activeVault.printTags(std::cout);
        }
    } else {
        handleInvalidCommand("Invalid vault command.");
    }
}

/**
    Prints command usage and options to the console in a standard format.
    Standard format taken from: http://docopt.org/

    Usage:
        clam --vault (add --name <vault-name> --key <vault-key>
            | update --key <vault-key> --knew <new-key>
            | switch --name <vault-name> --key <vault-key>
            | delete --name <vault-name> --key <vault-key>
            | list [--key <vault-key> [--info]])
        clam --print <account-name> --key <vault-key> [--username | --password | --note]
        clam --clip <account-name> --key <vault-key> --username | --password
        clam --update <account-name> --key <vault-key> (--username <username> | --password <password> | --note <note> | --file <file-path> | --delete)
        clam --add <account-name> --key <vault-key> [--file <file-path> | --username <username> --password <password>]

    Options:
        -v, --vault=vault-opt           vault command (options are: add, update, switch, delete, or list)
                                            add: Create a new vault
                                            update: Update the key for the active vault.
                                            switch: Switch to a different vault.
                                            delete: Delete a vault (cannot be the active vault).
                                            list: List vault names or account names/info (for the active vault).
        -k, --key=vault-key             vault key used to encrypt/decrypt data stored in the given vault
        -n, --name=name                 name of vault or account
        --un, --username[=username]     account username flag or input
        --pw, --password[=password]     account password flag or input
        --note[=note]                   account note flag or input
        -c, --clip                      Copy an account's username or password to the clipboard.
        -p, --print                     Print some account information to the console.
        -u, --update                    Update some information for some account in the active vault.
        --knew, --newkey=new-key        new vault key input                 
        -f, --file=acct-filepath        Path to the file containing unencrypted account data to load.
        -f, --delete                    Delete an account from the active vault.
        -a, --add                       Add a new account to the active vault.
        -i, --info                      List info for all accounts in the active vault.
        -h, --help                      Display usage and options for this program.

    Additional documentation and source code can be found at: https://github.com/Kylefc64/pw-manager-lite
*/
void processHelpCommand() {
    std::cout << "Usage:\n"
        << "    clam --vault (add --name <vault-name> --key <vault-key>\n"
        << "        | update --key <vault-key> --knew <new-key>\n"
        << "        | switch --name <vault-name> --key <vault-key>\n"
        << "        | delete --name <vault-name> --key <vault-key>\n"
        << "        | list [--key <vault-key> [--info]])\n"
        << "    clam --print <account-name> --key <vault-key> [--username | --password | --note]\n"
        << "    clam --clip <account-name> --key <vault-key> --username | --password\n"
        << "    clam --update <account-name> --key <vault-key> (--username <username>\n"
        << "                                                  | --password <password>\n"
        << "                                                  | --note <note>\n"
        << "                                                  | --file <file-path>\n"
        << "                                                  | --delete)\n"
        << "    clam --add <account-name> --key <vault-key> [--file <file-path>\n"
        << "                                               | --username <username> --password <password>]\n\n"

    << "Options:\n"
    << "-v, --vault=vault-opt           vault command (options are: add, update, switch, delete, or list)\n"
    << "                                    add: Create a new vault\n"
    << "                                    update: Update the key for the active vault.\n"
    << "                                    switch: Switch to a different vault.\n"
    << "                                    delete: Delete a vault (cannot be the active vault).\n"
    << "                                    list: List vault names or account names/info (for the active vault).\n"
    << "-k, --key=vault-key             vault key used to encrypt/decrypt data stored in the given vault\n"
    << "-n, --name=name                 name of vault or account\n"
    << "--un, --username[=username]     account username flag or input\n"
    << "--pw, --password[=password]     account password flag or input\n"
    << "--note[=note]                   account note flag or input\n"
    << "-c, --clip                      Copy an account's username or password to the clipboard.\n"
    << "-p, --print                     Print some account information to the console.\n"
    << "-u, --update                    Update some information for some account in the active vault.\n"
    << "--knew, --newkey=new-key        new vault key input\n"
    << "-f, --file=acct-filepath        Path to the file containing unencrypted account data to load.\n"
    << "-f, --delete                    Delete an account from the active vault.\n"
    << "-a, --add                       Add a new account to the active vault.\n"
    << "-i, --info                      List info for all accounts in the active vault.\n"
    << "-h, --help                      Display usage and options for this program.\n\n"

    << "Additional documentation and source code can be found at:\n"
        << "    https://github.com/Kylefc64/pw-manager-lite" << std::endl;
}

/**
    Processes a command that pertains to some account in the currently active vault.
*/
void processAccountCommand(const CommandLineParser& commandOpts, VaultManager &vaultManager) {
    Utils::debugPrint(std::cout, "Entered processAccountCommand\n");

    std::string vaultKey = getVaultKey(commandOpts);

    if (vaultManager.empty()) {
        if (commandOpts.containsOpt(CommandLineOptions::ADD_OPTION)) {
            // Create a "default" vault to allow the user to add an account without first explicitly
            // creating a vault. The default vault's password is the password provided to the
            // add account command.

            vaultManager.addVault("default_vault", vaultKey);
        } else {
            std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
            return;
        }
    }

    // Verify that vaultKey is correct and report error and exit if not:
    if (!VaultManager::validateKey(vaultKey, vaultManager.activeVaultInfo().vaultSkeySalt, vaultManager.activeVaultInfo().vaultSkeyHash)) {
        return;
    }

    // Attempt to load and decrypt vault:
    Vault activeVault(vaultManager.getVaultDir(), vaultManager.activeVaultInfo().vaultName, vaultKey);
    if (commandOpts.containsOpt(CommandLineOptions::PRINT_OPTION)) {
        processAccountPrintCommand(commandOpts, activeVault);
    } else if (commandOpts.containsOpt(CommandLineOptions::CLIP_OPTION)) {
        processAccountClipCommand(commandOpts, activeVault);
    } else if (commandOpts.containsOpt(CommandLineOptions::UPDATE_OPTION)) {
        processAccountUpdateCommand(commandOpts, activeVault);
    } else if (commandOpts.containsOpt(CommandLineOptions::ADD_OPTION)) {
        processAccountAddCommand(commandOpts, activeVault);
    } else {
        handleInvalidCommand("Invalid account command.");
    }
}

/**
    Processes a print command. Assumes the active vault has successfully been decrypted.
*/
void processAccountPrintCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
    Utils::debugPrint(std::cout, "Entered processAccountPrintCommand\n");

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::PRINT_OPTION);
    
    std::optional<Account *> optAccount = activeVault.getAccount(accountName);
    if (!optAccount.has_value()) {
        return;
    }

    Account *account = optAccount.value();

    if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION)) {
        std::cout << account->getUsername() << std::endl;
    } else if (commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
        std::cout << account->getPassword() << std::endl;
    } else if (commandOpts.containsOpt(CommandLineOptions::NOTE_OPTION)) {
        std::cout << account->getNote() << std::endl;
    } else {
        std::cout << "un=" << account->getUsername() << '\n'
            << "pw=" << account->getPassword() << '\n'
            << "note=" << account->getNote() << std::endl;
    }
}

/**
    Processes a clip command. Assumes the active vault has successfully been decrypted.

    Resources:
        https://stackoverflow.com/questions/6436257/how-do-you-copy-paste-from-the-clipboard-in-c
        https://stackoverflow.com/questions/40436045/in-qt-how-can-i-register-a-qstring-to-my-systems-clipboard-both-quoted-and-no/40437290#40437290
        https://linoxide.com/linux-how-to/copy-paste-commands-output-xclip-linux/
*/
void processAccountClipCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
    Utils::debugPrint(std::cout, "Entered processAccountClipCommand\n");

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::CLIP_OPTION);
    
    std::optional<Account *> optAccount = activeVault.getAccount(accountName);
    if (!optAccount.has_value()) {
        return;
    }

    Account *account = optAccount.value();

    // TODO: How to get xclip to not add a newline to end of copied string??
    if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION)) {
        system(("echo " + account->getUsername() + " | xclip -selection c").c_str());
    } else if (commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
        system(("echo " + account->getPassword() + " | xclip -selection c").c_str());
    } else {
        handleInvalidCommand("Invalid clip option.");
    }
    return;
}

/**
    Processes an update command. Assumes the active vault has successfully been decrypted.
*/
void processAccountUpdateCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
    Utils::debugPrint(std::cout, "Entered processAccountUpdateCommand\n");

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::UPDATE_OPTION);
    
    std::optional<Account *> optAccount = activeVault.getAccount(accountName);
    if (!optAccount.has_value()) {
        return;
    }

    Account *account = optAccount.value();

    if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION)) {
        std::string username = commandOpts.getOpt(CommandLineOptions::USERNAME_OPTION);
        // Update the username of the given account
        account->setUsername(username);
    } else if (commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
        std::string password = commandOpts.getOpt(CommandLineOptions::PASSWORD_OPTION);
        // Update the password of the given account
        account->setPassword(password);
    } else if (commandOpts.containsOpt(CommandLineOptions::NOTE_OPTION)) {
        std::string note = commandOpts.getOpt(CommandLineOptions::NOTE_OPTION);
        // Update the note of the given account
        account->setNote(note);
    } else if (commandOpts.containsOpt(CommandLineOptions::FILE_OPTION)) {
        std::string filePath = commandOpts.getOpt(CommandLineOptions::FILE_OPTION);
        // Update all details of the given account
        Account newAccount(accountName);
        if (newAccount.loadFromFile(filePath)) {
            *account = newAccount;
        }
    } else if (commandOpts.containsOpt(CommandLineOptions::DELETE_OPTION)) {
        activeVault.removeAccount(accountName);
    } else {
        handleInvalidCommand("Invalid account update option.");
    }

    activeVault.writeVault();
}

/**
    Processes an add command. Assumes the active vault has successfully been decrypted.
*/
void processAccountAddCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
    Utils::debugPrint(std::cout, "Entered processAccountAddCommand\n");

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::ADD_OPTION);

    if (commandOpts.containsOpt(CommandLineOptions::FILE_OPTION)) {
        std::string filePath = commandOpts.getOpt(CommandLineOptions::FILE_OPTION);
        // Read the new account from the specified file
        Account account(accountName);
        if (account.loadFromFile(filePath)) {
            activeVault.addAccount(account);
        }
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
