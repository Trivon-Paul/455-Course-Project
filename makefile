CC = gcc
CFLAGS = -Wall

caesar: caesar.c
	$(CC) $(CFLAGS) -o caesar caesar.

clean:
	rm -f caesar
