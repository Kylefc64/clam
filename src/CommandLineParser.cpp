#include "CommandLineParser.h"

#include <vector>

#ifdef __linux__
#include <getopt.h>
#endif
#ifdef _WIN32
#include "getopt/getopt.h"
#endif
#include <iostream>
// getopt and getopt_long documentation: https://www.gnu.org/software/libc/manual/html_node/Getopt.html
// how to avoid having to use "=" for options with optional arguments: https://stackoverflow.com/questions/1052746/getopt-does-not-parse-optional-arguments-to-parameters

CommandLineParser::CommandLineParser(int argc, char **argv) {
    
    struct option long_options[] = {
        {"vault",    required_argument, 0, CommandLineOptions::VAULT_OPTION},
        {"key",    required_argument, 0, CommandLineOptions::KEY_OPTION},
        {"name",    required_argument, 0, CommandLineOptions::NAME_OPTION},
        {"username",    optional_argument, 0, CommandLineOptions::USERNAME_OPTION},
        {"un",    optional_argument, 0, CommandLineOptions::USERNAME_OPTION},
        {"password",    optional_argument, 0, CommandLineOptions::PASSWORD_OPTION},
        {"pw",    optional_argument, 0, CommandLineOptions::PASSWORD_OPTION},
        {"note",    optional_argument, 0, CommandLineOptions::NOTE_OPTION},
        {"clip",    required_argument, 0, CommandLineOptions::CLIP_OPTION},
        {"print",    required_argument, 0, CommandLineOptions::PRINT_OPTION},
        {"update",    required_argument, 0, CommandLineOptions::UPDATE_OPTION},
        {"newkey",    required_argument, 0, CommandLineOptions::NEWKEY_OPTION},
        {"knew",    required_argument, 0, CommandLineOptions::NEWKEY_OPTION},
        {"file",    required_argument, 0, CommandLineOptions::FILE_OPTION},
        {"delete",    no_argument, 0, CommandLineOptions::DELETE_OPTION},
        {"add",    required_argument, 0, CommandLineOptions::ADD_OPTION},
        {"info",    no_argument, 0, CommandLineOptions::INFO_OPTION},
        {"help",    no_argument, 0, CommandLineOptions::HELP_OPTION},
        {0, 0, 0, 0}
    };

    int c;
    const char *optoptarg; // optional option argument

    while (1) {
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "v:k:n:c:p:u:f:a:dih",
                       long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            // This case should never occur...

            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case CommandLineOptions::VAULT_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::VAULT_OPTION, optarg));
            break;

        case CommandLineOptions::KEY_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::KEY_OPTION, optarg));
            break;

        case CommandLineOptions::NAME_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::NAME_OPTION, optarg));
            break;

        case CommandLineOptions::USERNAME_OPTION:
            if(!optarg && NULL != argv[optind] && '-' != argv[optind][0] ) {
                // If optarg isn't set and argv[optind] doesn't look like another option,
                // then treat argv[optind] as the parameter to this optional option:
                optoptarg = argv[optind];
            } else {
                optoptarg = "";
            }
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::USERNAME_OPTION, optoptarg));
            break;

        case CommandLineOptions::PASSWORD_OPTION:
            if(!optarg && NULL != argv[optind] && '-' != argv[optind][0] ) {
                // If optarg isn't set and argv[optind] doesn't look like another option,
                // then treat argv[optind] as the parameter to this optional option:
                optoptarg = argv[optind];
            } else {
                optoptarg = "";
            }
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::PASSWORD_OPTION, optoptarg));
            break;

        case CommandLineOptions::NOTE_OPTION:
            if(!optarg && NULL != argv[optind] && '-' != argv[optind][0] ) {
                // If optarg isn't set and argv[optind] doesn't look like another option,
                // then treat argv[optind] as the parameter to this optional option:
                optoptarg = argv[optind];
            } else {
                optoptarg = "";
            }
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::NOTE_OPTION, optoptarg));
            break;

        case CommandLineOptions::CLIP_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::CLIP_OPTION, optarg));
            break;

        case CommandLineOptions::PRINT_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::PRINT_OPTION, optarg));
            break;

        case CommandLineOptions::UPDATE_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::UPDATE_OPTION, optarg));
            break;

        case CommandLineOptions::NEWKEY_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::NEWKEY_OPTION, optarg));
            break;

        case CommandLineOptions::FILE_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::FILE_OPTION, optarg));
            break;

        case CommandLineOptions::DELETE_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::DELETE_OPTION, ""));
            break;

        case CommandLineOptions::ADD_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::ADD_OPTION, optarg));
            break;

        case CommandLineOptions::INFO_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::INFO_OPTION, ""));
            break;

        case CommandLineOptions::HELP_OPTION:
            optMap.insert(std::pair<CommandLineOptions, std::string>(CommandLineOptions::HELP_OPTION, ""));
            break;

        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            abort();
        }
    }
}

bool CommandLineParser::containsOpt(CommandLineOptions opt) const {
    return optMap.find(opt) != optMap.end();
}

std::string CommandLineParser::getOpt(CommandLineOptions opt) const {
    if (optMap.find(opt) != optMap.end()) {
        return optMap.at(opt);
    } else {
        return "";
    }
}
