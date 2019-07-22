# pw-manager-lite
A light, terminal-based application for securely managing all of your account passwords

# Usage Options

1. Vault options
* pml -v add -n \<new vault's name\> -k \<new vault's key\>
    * Creates a new vault with the given key
* pml -v update -k \<vault's old key\> -knew \<vault's new key\>
    * Updates the active vault's key to the given new key
* pml -v switch -n \<name of vault to switch to\> -k \<vault's key\>
    * Switches the active vault to the given vault
* pml -v delete -n \<name of vault to delete (cannot be active vault)\> -k \<vault's key\>
    * Deletes the specified vault
* pml -v list
    * Lists the names of all vaults
* pml -v list -k \<vault key\>
    * Lists the names of all accounts in the active vault
* pml -v list -k \<vault key\> -i
    * Lists the account information for all accounts in the active vault in a human-readable format

2. Print options
* pml -p \<acct name\> -k \<vault key\>
    * prints (nicely formatted) all details of the given account in the active vault
* pml -p \<acct name\> -k \<vault key\> -un
    * prints only the username of the given account in the active vault
* pml -p \<acct name\> -k \<vault key\> -pw
    * prints only the password of the given account in the active vault
* pml -p \<acct name\> -k \<vault key\> -note
    * prints only the notes of the given account in the active vault

3. Clip options
* pml -c \<acct name\> -k \<vault key\> -un
    * clips only the username of the given account in the active vault
* pml -c \<acct name\> -k \<vault key\> -pw
    * clips only the password of the given account in the active vault

4. Update options
* pml -u \<acct name\> -k \<vault key\> -un \<new username\>
    * Updates the given account of the active vault's username to the given username
* pml -u \<acct name\> -k \<vault key\> -pw \<new password\>
    * Updates the given account of the active vault's password to the given password
* pml -u \<acct name\> -k \<vault key\> -note \<new note\>
    * Updates the given account of the active vault's note to the given note
* pml -u \<acct name\> -k \<vault key\> -f \<file path\>
    * Updates all of the given account of the active vault's details to the contents of the specified file,
      which must be formatted as follows:
      un=\<username\>\<newline\>
      ps=\<password\>\<newline\>
      \<note\>
* pml -u \<acct name\> -k \<vault key\> -d
    * deletes the record for the given account in the active vault

5. Add options
* pml -a \<acct name\> -k \<vault key\> -f \<file path\>
    * Add a new account which's details are parsed from the specified file (same format as above) to the active vault
* pml -a \<acct name\> -k \<vault key\>
    * Adds a new account with no details to the active vault
* pml -a \<acct name\> -k \<vault key\> -un \<username\> -pw \<password\>
    * Add a new account with the given username & password to the active vault


## Application Dependencies:
* libtomcrypt (libtomcrypt-dev package)
* xclip
