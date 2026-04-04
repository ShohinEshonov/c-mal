CC = gcc
CFLAGS = -Wall -Wextra -g


TARGET = lilo


SRCS = $(wildcard src/*.c)


OBJS = $(SRCS:src/%.c=build/%.o)


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET) 
