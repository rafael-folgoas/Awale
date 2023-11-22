CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = monprogramme
SRC = jeu.c main.c joueur.c

$(TARGET): $(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET) *.o


