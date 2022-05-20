CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=*.c
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
EXECUTABLE=crabshell


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(@)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $(@)

clean:
	rm -f *.o
	rm -f $(EXECUTABLE)
