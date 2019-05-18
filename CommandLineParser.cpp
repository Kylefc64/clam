#include "CommandLineParser.h"

#include <vector>

CommandLineParser::CommandLineParser(int argc, char **argv) {
	std::vector<std::string> tokens;
	for (int i = 1; i < argc; ++i) {
		tokens.push_back(std::string(argv[i]));
	}

	for (int i = 0; i < tokens.size(); ++i) {
		if ('-' == tokens[i][0]) {
			if (((i + 1) == tokens.size()) || ('-' == tokens[i+1][0])) {
				// Insert a no-param argument into the map
				argMap.insert(std::pair<std::string, std::string>(tokens[i], ""));
			} else {
				argMap.insert(std::pair<std::string, std::string>(tokens[i], tokens[i+1]));
				i++;
			}
		}
	}
}

bool CommandLineParser::containsArg(const std::string& arg) {
	return argMap.find(arg) != argMap.end();
}

std::string CommandLineParser::getArg(const std::string& arg) {
	if (argMap.find(arg) != argMap.end()) {
		return argMap.at(arg);
	} else {
		return "";
	}
}
