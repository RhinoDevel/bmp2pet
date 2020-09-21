# MT, 2020sep21
#
# - Original source: http://mrbook.org/blog/tutorials/make/

CC=gcc

#CFLAGS=-c -O3 -std=gnu11 -Wall -DNDEBUG
CFLAGS=-c -g -std=gnu11 -Wall
LDFLAGS=
SOURCES=main.c Sys.c Bmp.c Dim.c FileSys.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bmp2pet

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
