all: kilo

kilo: kilo.o
	$(CC) -o $@ $^ -Wall -Wextra -pedantic -std=c99

kilo.o: kilo.c 
	$(CC) -c $^

clean: 
	rm kilo.o kilo