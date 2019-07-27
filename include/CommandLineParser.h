#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <map>

enum CommandLineOptions {
	VAULT_OPTION = 'v', // -v or --vault
	KEY_OPTION = 'k', // -k or --key
	NAME_OPTION = 'n', // -n or --name
	USERNAME_OPTION = 'z' + 1000, // --un or --username
	PASSWORD_OPTION = 'z' + 1001, // --pw or --password
	NOTE_OPTION = 'z' + 1002, // --note
	CLIP_OPTION = 'c', // -c or --clip
	PRINT_OPTION = 'p', // -p or --print
	UPDATE_OPTION = 'u', // -u or --update
	NEWKEY_OPTION = 'z' + 1003, // --knew or --newkey
	FILE_OPTION = 'f', // -f or --file
	DELETE_OPTION = 'd', // -d or --delete
	ADD_OPTION = 'a', // -a or --add
	INFO_OPTION = 'i', // -i or --info
	HELP_OPTION = 'h',
};

class CommandLineParser {
public:
	CommandLineParser(int argc, char **argv);
	bool containsOpt(CommandLineOptions opt) const;
	std::string getOpt(CommandLineOptions opt) const;
private:
	std::map<CommandLineOptions, std::string> optMap;
};

#endif
