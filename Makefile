CC = gcc
CFLAGS = -O3 -march=native

all: spy victim

spy: spy.c cacheutility.h
	$(CC) $(CFLAGS) -o spy spy.c 

victim: victim.c cacheutility.h
	$(CC) $(CFLAGS) -o victim victim.c 

clean:
	rm -f spy victim