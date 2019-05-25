#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <map>

class CommandLineParser {
public:
	CommandLineParser(int argc, char **argv);
	bool containsArg(const std::string& arg) const;
	std::string getArg(const std::string& arg) const;
private:
	std::map<std::string, std::string> argMap;
};

#endif
