all: src/main.cpp src/CommandLineParser.h src/CommandLineParser.cpp src/Vault.h src/Vault.cpp src/Account.h src/Account.cpp src/Utils.h src/Utils.cpp
	g++ -std=c++11 -g -o build/pml src/main.cpp src/CommandLineParser.cpp src/Vault.cpp src/Account.cpp src/Utils.cpp -ltomcrypt
clean:
	rm build/pml
run:
	cd build; ./pml
.PHONY:
	clean run
