
all: main

main: mem.c main.c
	gcc -g -Wall -Wextra -Werror -std=gnu99 -o main mem.c main.c 

clean:
	rm -f *.o  main
