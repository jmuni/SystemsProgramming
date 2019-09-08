CC = gcc -Wall -Werror -fsanitize=address

memgrind: memgrind.o mymalloc.o
	$(CC) memgrind.o mymalloc.o -o memgrind

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) -c mymalloc.c

memgrind.o: memgrind.c mymalloc.h
	$(CC) -c memgrind.c

clean:
	rm -f memgrind mymalloc.o memgrind.o

	

