#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <map>

// getopt and getopt_long documentation: https://www.gnu.org/software/libc/manual/html_node/Getopt.html

enum CommandLineOptions {
	VAULT_OPTION, // -v or --vault
	KEY_OPTION, // -k or --key
	NAME_OPTION, // -n or --name
	USERNAME_OPTION, // --un or --username
	PASSWORD_OPTION, // --pw or --password
	NOTE_OPTION, // --note
	CLIP_OPTION, // -c or --clip
	PRINT_OPTION, // -p or --print
	UPDATE_OPTION, // -u or --update
	NEWKEY_OPTION, // --knew or --newkey
	FILE_OPTION, // -f or --file
	DELETE_OPTION, // -d or --delete
	ADD_OPTION, // -a or --add
	INFO_OPTION, // -i or --info
};

class CommandLineParser {
public:
	CommandLineParser(int argc, char **argv);
	bool containsOpt(const std::string& arg) const;
	std::string getOpt(const std::string& arg) const;
private:
	std::map<std::string, std::string> argMap;
};

#endif
