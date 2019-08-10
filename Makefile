SRC_DIR=src/
INCLUDE_DIR=src/
BUILD_DIR=build/
BIN_DIR=bin/
INSTALL_DIR=/usr/local/bin/
TEST_DIR=test/
CXX=g++
CXXFLAGS=-std=c++17 -g -I include
LIBFLAGS=-ltomcrypt
PROG_NAME=clam
TEST=$(TEST_DIR)run_tests.py
PROG=$(BIN_DIR)$(PROG_NAME)
OBJS=$(BUILD_DIR)main.o $(BUILD_DIR)CommandLineParser.o $(BUILD_DIR)Vault.o $(BUILD_DIR)Account.o $(BUILD_DIR)Utils.o $(BUILD_DIR)VaultManager.o

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS) $(LIBFLAGS)

$(BUILD_DIR)main.o: $(SRC_DIR)main.cpp $(INCLUDE_DIR)CommandLineParser.h $(INCLUDE_DIR)Vault.h $(INCLUDE_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)main.cpp -o $@

$(BUILD_DIR)CommandLineParser.o: $(SRC_DIR)CommandLineParser.cpp $(INCLUDE_DIR)CommandLineParser.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)CommandLineParser.cpp -o $@

$(BUILD_DIR)Vault.o: $(SRC_DIR)Vault.cpp $(INCLUDE_DIR)Vault.h $(INCLUDE_DIR)Account.h $(INCLUDE_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)Vault.cpp -o $@

$(BUILD_DIR)Account.o: $(SRC_DIR)Account.cpp $(INCLUDE_DIR)Account.h $(INCLUDE_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)Account.cpp -o $@

$(BUILD_DIR)Utils.o: $(SRC_DIR)Utils.cpp $(INCLUDE_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)Utils.cpp -o $@

$(BUILD_DIR)VaultManager.o: $(SRC_DIR)VaultManager.cpp $(INCLUDE_DIR)VaultManager.h $(INCLUDE_DIR)Account.h $(INCLUDE_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)VaultManager.cpp -o $@

install: $(PROG)
	cp $(PROG) $(INSTALL_DIR); chmod +x $(INSTALL_DIR)$(PROG_NAME)

uninstall: $(PROG)
	rm -rf $(INSTALL_DIR)$(PROG_NAME)

get-dependencies:
	apt-get install libtomcrypt-dev; apt install xclip

remove-dependencies:
	apt-get autoremove libtomcrypt-dev; apt autoremove xclip

clean:
	rm $(PROG) $(OBJS)

test: $(PROG) $(TEST)
        ifneq (, $(shell which python3))
		python3 $(TEST)
        else ifneq (, $(shell which py))
		py $(TEST)
        else ifneq (, $(shell which python))
        	$(error "test function requires python 3")
        endif

run: $(PROG)
	./$(PROG)

.PHONY:
	install uninstall clean test run get-dependencies remove-dependencies
