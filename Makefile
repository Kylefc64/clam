pml: main.cpp CommandLineParser.h CommandLineParser.cpp Vault.h Vault.cpp Account.h Account.cpp
	g++ -g -o pml main.cpp CommandLineParser.cpp Vault.cpp Account.cpp
clean:
	rm pml