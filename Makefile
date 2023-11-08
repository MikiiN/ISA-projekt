CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
FILES = src/*.cpp src/headers/*.hpp
TARGET = isa-ldapserver 

$(TARGET): $(FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(FILES)

clean:
	rm -rf $(TARGET)
