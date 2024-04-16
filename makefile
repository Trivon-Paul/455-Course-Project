CC = gcc
CFLAGS = -Wall

caesar: caesar.c
	$(CC) $(CFLAGS) -o caesar caesar.c

clean:
	rm -f caesar
