CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
OBJS = src/main.o src/args.o src/probe.o src/triage.o src/banner.o src/cidr.o src/threadpool.o

rapido: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o rapido
