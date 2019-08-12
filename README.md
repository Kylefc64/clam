## CLAM (Command Line Account Manager)
CLAM is a command line tool that allows you to easily store all of your account information encrypted in one place, and to decrypt and retrieve this information via a simple CLI.

Our motivations for making this tool:
* Desire to only need to remember one strong password, rather than multiple weak (and often repeated) passwords for multiple accounts
  * This single strong password can be used to retrieve each individual account password (and username) from an encrypted vault on the fly
* Desire for a command-line, rather than a graphical interface for locally managing all of our account usernames and passwords
* Desire to create and deploy our own cryptographic tool because cryptography is fun

## Features
* Simple CLI for multi-account username and password storage and retrieval
* Commands to decrypt and copy account usernames or passwords to the clipboard for easily and quickly logging into your accounts
* Commands to decrypt and print specific account information to the console
* Commands to add accounts, update account information, and delete accounts
* Commands to create vaults, update vault passwords, switch between vaults, and remove vaults

## Dependencies
CLAM requires that the following libraries are installed on your system:
* libtomcrypt (package name is libtomcrypt-dev)
* libx11-dev

## Bulding and Installing CLAM
To build CLAM, run the following commands:
* `rm -rf build && mkdir build && cd build`
* `cmake -D CLIP_EXAMPLES=OFF -D CLIP_TESTS=OFF -D CLIP_X11_WITH_PNG=OFF ..`
* `make`

To install CLAM, run the following command:
* `make install`

By default, this will install CLAM to /usr/local/bin/, but the install location can be modified by editing the CMakeLists.txt file prior to running cmake.

## Use-case Examples
I have three accounts - Chase, GitHub, and Facebook. I am tired of repeatedly forgetting my usernames and passwords for these accounts, so I decide to use CLAM to help me manage my account information. I start by coming up with a single strong key that I will use to encrypt and decrypt all of my account data. I choose the key `7sDFgS$DF5&a.` I then add all of my accounts to CLAM using this key with the following commands:
* `clam --add chase --key 7sDFgS$DF5&a --username chase_uname123 --password chase_pw123`
  * (or `clam -a chase -k 7sDFgS$DF5&a --un chase_uname123 --pw chase_pw123`)
* `clam --add github --key 7sDFgS$DF5&a --username github_uname123 --password github_pw123`
  * (or `clam -a github -k 7sDFgS$DF5&a --un github_uname123 --pw github_pw123`)
* `clam --add facebook --key 7sDFgS$DF5&a --username facebook_uname123 --password facebook_pw123`
  * (or `clam -a facebook -k 7sDFgS$DF5&a --un facebook_uname123 --pw facebook_pw123`)

Also, Chase asked me to answer a couple of security questions, but I'm not sure I would remember the answers if I had to answer them, so I can also store this information along with my username and password for Chase:
* `clam --update chase --key 7sDFgS$DF5&a --note "question1: Idaho; question2: Rachel"`
  * (or `clam -u chase -k 7sDFgS$DF5&a --note "question1: Idaho; question2: Rachel"`)

Now, when I need to login to one of my accounts (e.g. GitHub), I can simply run the following commands to retrieve my username or password:
* `clam --clip github --key 7sDFgS$DF5&a --username`
  * (or `clam -c github -k 7sDFgS$DF5&a --un`)
* `clam --clip github --key 7sDFgS$DF5&a --password`
  * (or `clam -c github -k 7sDFgS$DF5&a --pw`)

My GitHub username or password can then simply be pasted into the GitHub login screen.

Also, if I ever need to recall the answers to my Chase security questions, I can run the following command:
* `clam --print chase --key 7sDFgS$DF5&a --note`
  * (or `clam -p chase -k 7sDFgS$DF5&a --note`)

Oops, my key has somehow been compromised. I can easily update my key using the following command:
* `clam --vault update --key 7sDFgS$DF5&a --newkey mynewkey56789`
  * (or `clam -v update -k 7sDFgS$DF5&a --newkey mynewkey56789`)

Also, my Facebook password has been compromised, so I will need to update my Facebook password in CLAM as well:
* `clam --update facebook --key 7sDFgS$DF5&a --password mynewfbpassword789`
  * (or `clam -u facebook -k 7sDFgS$DF5&a --pw mynewfbpassword789`)

A detailed list of all available commands along with brief descriptions for each can be found in the usage_examples.md file.

## FAQ
Q: What is a vault?<br/>
A: A vault is a collection of account information that is encrypted and decrypted using the same key. CLAM provides commands for creating and managing multiple vaults, although a default vault is created automatically when creating your first account.

Q: Where is my encrypted data stored?<br/>
A: All of your data is stored locally, in a directory labeled ".clam" in your home directory.

Q: What kind of encryption is used to store my data?<br/>
A: CLAM uses 256-bit AES CTR encryption that is implemented by the libtomcrypt library.
