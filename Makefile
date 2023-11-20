CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
OBJ_FOLDER = ./obj
OBJECTS = $(OBJ_FOLDER)/argumentParser.o $(OBJ_FOLDER)/ber.o $(OBJ_FOLDER)/database.o $(OBJ_FOLDER)/error.o $(OBJ_FOLDER)/LDAPserver.o
SRC_FOLDER = ./src
HEADER_FOLDER = ./src/headers
TARGET = isa-ldapserver 

$(TARGET): $(OBJECTS) $(SRC_FOLDER)/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_FOLDER)/argumentParser.o: $(SRC_FOLDER)/argumentParser.cpp $(HEADER_FOLDER)/argumentParser.hpp $(HEADER_FOLDER)/error.hpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_FOLDER)/ber.o: $(SRC_FOLDER)/ber.cpp $(HEADER_FOLDER)/ber.hpp $(HEADER_FOLDER)/filter.hpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_FOLDER)/database.o: $(SRC_FOLDER)/database.cpp $(HEADER_FOLDER)/database.hpp $(HEADER_FOLDER)/filter.hpp $(HEADER_FOLDER)/error.hpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_FOLDER)/error.o: $(SRC_FOLDER)/error.cpp $(HEADER_FOLDER)/error.hpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_FOLDER)/LDAPserver.o: $(SRC_FOLDER)/LDAPserver.cpp $(HEADER_FOLDER)/LDAPserver.hpp $(HEADER_FOLDER)/error.hpp $(HEADER_FOLDER)/ber.hpp $(HEADER_FOLDER)/database.hpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(TARGET) $(OBJECTS)
