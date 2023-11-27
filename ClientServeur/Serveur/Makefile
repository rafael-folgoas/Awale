CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = server
SRC = jeu.c files.c server2.c

$(TARGET): $(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET) *.o


