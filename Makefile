SRCS = *.c
OBJS = $(patsubst %.c,%.o,$(SRCS))

kilo: $(OBJS)
	$(CC) -o $@ $^ -Wall -Wextra -pedantic -std=c99
%.o:	%.c
	$(CC) -c $^
clean:
	rm *.o kilo
