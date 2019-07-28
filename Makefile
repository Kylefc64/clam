SRC_DIR=src/
INCLUDE_DIR=include/
BUILD_DIR=build/
BIN_DIR=bin/
TEST_DIR=test/
CXX=g++
CXXFLAGS=-std=c++17 -g -I include
LIBFLAGS=-ltomcrypt
PROG=$(BIN_DIR)pml
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

clean:
	rm $(PROG) $(OBJS) $(TEST_DIR)pml

test: $(PROG)
	cp $(PROG) $(TEST_DIR); cd $(TEST_DIR); python3 run_tests.py

run: $(PROG)
	./$(PROG)

.PHONY:
	clean run clean
