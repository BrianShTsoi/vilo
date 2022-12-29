SRCS = *.c
OBJS = $(patsubst %.c,%.o,$(SRCS))

vilo: $(OBJS)
	$(CC) -o $@ $^ -Wall -Wextra -pedantic -std=c99
%.o:	%.c
	$(CC) -c $^
clean:
	rm *.o vilo
