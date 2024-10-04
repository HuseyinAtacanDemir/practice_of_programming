# Compiler
CC = gcc

# Compiler flags for normal and debug modes
CFLAGS = -Wall
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2

# Include directories (if your headers are in the same directory)
INCLUDES = -I.

# Source files for the main build
MAIN_SRCS = freq.main.c freq.internal.c eprintf.c hash.c list.c

# Source files for unit tests (including test-specific sources)
TEST_SRCS = freq.test.c freq.internal.c eprintf.c hash.c list.c

# Object files for the normal build
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

# Object files for the test build
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Headers (for future-proofing if you need to add more)
HDRS = freq.internal.h hash.h list.h eprintf.h

# Target executables
MAIN_TARGET = freq
TEST_TARGET = freq_test

# Default target (builds the main executable)
all: $(MAIN_TARGET)

# Debug mode target
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(MAIN_TARGET)

# Release mode target
release: CFLAGS += $(RELEASE_FLAGS)
release: $(MAIN_TARGET)

# Pattern rule to compile .c files into .o object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Rule to build the main target
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN_TARGET) $(MAIN_OBJS)

# Rule to build the unit test target
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_TARGET) $(TEST_OBJS)

# Integration test mode (compiles and runs test script)
itest: all
	./test.sh

# Unit test mode (compiles and runs unit tests)
utest: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean target to remove object files and executables
clean:
	rm -f $(MAIN_OBJS) $(TEST_OBJS) $(MAIN_TARGET) $(TEST_TARGET)

# Phony targets
.PHONY: all debug release clean itest utest

