pml: main.cpp CommandLineParser.h CommandLineParser.cpp Vault.h Vault.cpp Account.h Account.cpp Utils.h Utils.cpp
	g++ -std=c++11 -g -o pml main.cpp CommandLineParser.cpp Vault.cpp Account.cpp
clean:
	rm pml