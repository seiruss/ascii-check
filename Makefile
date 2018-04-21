CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
SRC = ascii_check.c
TARGET = ascii_check.out

.PHONY: all debug clean

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -s $(SRC) -o $@

debug:
	$(CC) $(CFLAGS) -g $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
