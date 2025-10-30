CC = gcc
CFLAGS = -flto -Wall -Wextra -O2 -I.
TARGET = librt.a

SRCS = src\rt_collections.c src\rt_thread.c src\rt.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET) clean

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJS) 2>nul

.PHONY: all clean