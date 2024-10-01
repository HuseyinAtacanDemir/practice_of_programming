# Compiler
CC = gcc

# Compiler flags for normal and debug modes
CFLAGS = -Wall
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2

# Include directories (if your headers are in the same directory)
INCLUDES = -I.

# Source files (including all relevant .c files)
SRCS = freq.c hash.c eprintf.c list.c

# Object files
OBJS = $(SRCS:.c=.o)

# Headers (for future-proofing in case you need to add more)
HDRS = hash.h list.h eprintf.h

# Target executable
TARGET = freq

# Default target (normal build)
all: $(TARGET)

# Debug mode target
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Normal mode target
release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

# Rule to test the target
test:
	./test.sh

# Clean target to remove object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all debug release test clean

