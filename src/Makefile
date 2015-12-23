CC=g++
CFLAGS=-c -Wall -std=c++14 -O2
LDFLAGS=-lpng -lpthread
SOURCES=main.cpp parseInput.cpp genFrameName.cpp globalVariables.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=voronoi

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(EXECUTABLE)
