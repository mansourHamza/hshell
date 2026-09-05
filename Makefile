CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

all: hshell

hshell: src/main.cpp src/shell.cpp include/shell.h
	$(CXX) $(CXXFLAGS) -o hshell src/main.cpp src/shell.cpp

clean:
	rm -f hshell
