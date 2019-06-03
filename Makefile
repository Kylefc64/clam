SRC_DIR=src/
BUILD_DIR=build/
CXX=g++
CXXFLAGS=-std=c++11 -g
LIBFLAGS=-ltomcrypt
PROG=$(BUILD_DIR)pml
OBJS=$(BUILD_DIR)main.o $(BUILD_DIR)CommandLineParser.o $(BUILD_DIR)Vault.o $(BUILD_DIR)Account.o $(BUILD_DIR)Utils.o

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS) $(LIBFLAGS)

$(BUILD_DIR)main.o: $(SRC_DIR)main.cpp $(SRC_DIR)CommandLineParser.h $(SRC_DIR)Vault.h $(SRC_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)main.cpp -o $@

$(BUILD_DIR)CommandLineParser.o: $(SRC_DIR)CommandLineParser.cpp $(SRC_DIR)CommandLineParser.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)CommandLineParser.cpp -o $@

$(BUILD_DIR)Vault.o: $(SRC_DIR)Vault.cpp $(SRC_DIR)Vault.h $(SRC_DIR)Account.h $(SRC_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)Vault.cpp -o $@

$(BUILD_DIR)Account.o: $(SRC_DIR)Account.cpp $(SRC_DIR)Account.h $(SRC_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)Account.cpp -o $@

$(BUILD_DIR)Utils.o: $(SRC_DIR)Utils.cpp $(SRC_DIR)Utils.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)Utils.cpp -o $@

clean:
	rm $(PROG) $(OBJS)

run:
	./$(PROG)

.PHONY:
	clean run clean
