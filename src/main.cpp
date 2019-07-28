#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

#include "CommandLineParser.h"
#include "Vault.h"
#include "Utils.h"
#include "VaultManager.h"

std::string getVaultKey(const CommandLineParser& commandOpts);
std::string getAccountName(const CommandLineParser& commandOpts, CommandLineOptions nameOpt);

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

    VaultManager vaultManager;

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
    Attempts to retrieve the name parameter from the command options and
    reports a generic error if the option does not exist, or if the account name
    does not exist in the active vault..
*/
std::string getAccountName(const CommandLineParser& commandOpts, CommandLineOptions nameOpt) {
    std::string name = commandOpts.getOpt(nameOpt);
    if (name == "") {
        std::cout << "Error: You must provide an account name using the -n option." << std::endl;
    }
    return name;
}

/**
    Process a command that pertains to an entire vault.
*/
void processVaultCommand(const CommandLineParser& commandOpts, VaultManager &vaultManager) { 
    Utils::debugPrint(std::cout, "Entered processVaultCommand\n");

    std::string metaCommand = commandOpts.getOpt(CommandLineOptions::VAULT_OPTION);
    if (metaCommand == "list" && !commandOpts.containsOpt(CommandLineOptions::KEY_OPTION)) {
        vaultManager.listVaultNames();
        return;
    }

    std::string vaultKey = getVaultKey(commandOpts);

    if (metaCommand == "add") {
        // Create a new vault:
        const std::string newVaultName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
        if (newVaultName == "") {
            std::cout << "Error: You must provide a vault name using the -n option." << std::endl;
            return;
        }
    
        Utils::debugPrint(std::cout, newVaultName + " newVaultname \n");
        vaultManager.addVault(newVaultName, vaultKey);
    } else if (metaCommand == "update") {
        // Error if there is no active vault:
        if (vaultManager.empty()) {
            std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
            return;
        }

        // Verify that vaultKey is correct and report error and exit if not
        std::string newVaultKey = commandOpts.getOpt(CommandLineOptions::NEWKEY_OPTION);
        if (newVaultKey == "") {
            std::cout << "Error: New vault key must be provided to update vault key" << std::endl;
            return;
        }

        vaultManager.updateActiveVaultKey(vaultKey, newVaultKey);
    } else if (metaCommand == "switch") {
        std::string vaultToSwitchToName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
        if (vaultToSwitchToName == "") {
            std::cout << "Error: Must provide the name of the vault to which you wish to switch to." << std::endl;
        }

        vaultManager.switchActiveVault(vaultKey, vaultToSwitchToName);     
    } else if (metaCommand == "delete") {
        // Error if there is no active vault:
        if (vaultManager.empty()) {
            std::cout << "Error: You must first create a vault using the -v add command." << std::endl;
            return;
        }

        std::string vaultToDeleteName = commandOpts.getOpt(CommandLineOptions::NAME_OPTION);
        vaultManager.deleteVault(vaultKey, vaultToDeleteName);

    } else if (metaCommand == "list") {
        if (!VaultManager::validateKey(vaultKey, vaultManager.activeVaultInfo().vaultSkeySalt, vaultManager.activeVaultInfo().vaultSkeyHash)) {
            return;
        }
        Vault activeVault(vaultManager.activeVaultInfo().vaultName, vaultKey);

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
    Prints command usage and options to the console in a standard format.
    Standard format taken from: http://docopt.org/

    Usage:
        pml --vault (add --name <vault-name> --key <vault-key>
            | update --key <vault-key> --knew <new-key>
            | switch --name <vault-name> --key <vault-key>
            | delete --name <vault-name> --key <vault-key>
            | list [--key <vault-key> [--info]])
        pml --print <account-name> --key <vault-key> [--username <username> | --password <password> | --note <note>]
        pml --clip <account-name> --key <vault-key> --username | --password
        pml --update <account-name> --key <vault-key> (--username <username> | --password <password> | --note <note> | --file <file-path> | --delete)
        pml --add <account-name> --key <vault-key> [--file <file-path> | --username <username> --password <password>]

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
        -f, --file
        -f, --delete                    Delete an account from the active vault.
        -a, --add                       Add a new account to the active vault.
        -i, --info                      List info for all accounts in the active vault.
        -h, --help                      Display usage and options for this program.

    Additional documentation and source code can be found at: https://github.com/Kylefc64/pw-manager-lite
*/
void processHelpCommand() {
    std::cout << "Usage:\n"
        << "    pml --vault (add --name <vault-name> --key <vault-key>\n"
        << "        | update --key <vault-key> --knew <new-key>\n"
        << "        | switch --name <vault-name> --key <vault-key>\n"
        << "        | delete --name <vault-name> --key <vault-key>\n"
        << "        | list [--key <vault-key> [--info]])\n"
        << "    pml --print <account-name> --key <vault-key> [--username <username>\n"
        << "                                            | --password <password>\n"
        << "                                            | --note <note>]\n"
        << "    pml --clip <account-name> --key <vault-key> --username | --password\n"
        << "    pml --update <account-name> --key <vault-key> (--username <username>\n"
        << "                                            | --password <password>\n"
        << "                                            | --note <note>\n"
        << "                                            | --file <file-path>\n"
        << "                                            | --delete)\n"
        << "    pml --add <account-name> --key <vault-key> [--file <file-path>\n"
        << "                                            | --username <username> --password <password>]\n\n"

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
    << "-f, --file                      TBD\n"
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
    Vault activeVault(vaultManager.activeVaultInfo().vaultName, vaultKey);
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

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::PRINT_OPTION);
    if(!activeVault.exists(accountName)) {
        std::cout << "Error: The specified account does not exist. You may create an account using the -a option." << std::endl;
        return;
    }

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

    Resources:
        https://stackoverflow.com/questions/6436257/how-do-you-copy-paste-from-the-clipboard-in-c
        https://stackoverflow.com/questions/40436045/in-qt-how-can-i-register-a-qstring-to-my-systems-clipboard-both-quoted-and-no/40437290#40437290
        https://linoxide.com/linux-how-to/copy-paste-commands-output-xclip-linux/
*/
void processAccountClipCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
    Utils::debugPrint(std::cout, "Entered processAccountClipCommand\n");

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::CLIP_OPTION);
    if(!activeVault.exists(accountName)) {
        std::cout << "Error: The specified account does not exist. You may create an account using the -a option." << std::endl;
        return;
    }

    // TODO: How to get xclip to not add a newline to end of copied string??
    if (commandOpts.containsOpt(CommandLineOptions::USERNAME_OPTION)) {
        system(("echo " + activeVault.getAccount(accountName).getUsername() + " | xclip -selection c").c_str());
    } else if (commandOpts.containsOpt(CommandLineOptions::PASSWORD_OPTION)) {
        system(("echo " + activeVault.getAccount(accountName).getPassword() + " | xclip -selection c").c_str());
    } else {
        std::cout << "Error: Invalid clip option. Valid options are -un and -pw." << std::endl;
    }

    std::cout << "Copied requested content to clipboard." << std::endl;
    return;
}

/**
    Processes an update command. Assumes the active vault has successfully been decrypted.
*/
void processAccountUpdateCommand(const CommandLineParser& commandOpts, Vault &activeVault) {
    Utils::debugPrint(std::cout, "Entered processAccountUpdateCommand\n");

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::UPDATE_OPTION);
    if(!activeVault.exists(accountName)) {
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
    } else if (commandOpts.containsOpt(CommandLineOptions::DELETE_OPTION)) {
        activeVault.removeAccount(accountName);
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

    std::string accountName = getAccountName(commandOpts, CommandLineOptions::ADD_OPTION);

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
