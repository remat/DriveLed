CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=DriveLed.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=DriveLed

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
