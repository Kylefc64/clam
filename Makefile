pml: main.cpp CommandLineParser.h CommandLineParser.cpp
	g++ -g -o pml main.cpp CommandLineParser.cpp
clean:
	rm pml