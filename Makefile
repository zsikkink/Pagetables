# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror -std=c11 -c

# Linker flags
LDFLAGS =

# Source files
SRCS = mlpt.c

# Object files
OBJS = $(SRCS:.c=.o)

# Library name
LIBRARY = libmlpt.a

# Default target
all: $(LIBRARY)

# Rule to create the library
$(LIBRARY): $(OBJS)
	ar rcs $@ $^

# Rule to compile source files
%.o: %.c mlpt.h config.h
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(LIBRARY)
