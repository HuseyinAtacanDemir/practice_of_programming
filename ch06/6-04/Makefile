# Compiler
CC = gcc

# Compiler flags for normal and debug modes
CFLAGS = -Wall
DEBUG_FLAGS = -g -DDEBUG
TEST_FLAGS = -g -DTEST
RELEASE_FLAGS = -O2

# Include directories (if your headers are in the same directory)
INCLUDES = -I.

# Source files
SRCS = freq.c freq.internal.c hash.c eprintf.c list.c

# Object files for the normal build
OBJS = $(SRCS:.c=.o)

# Object files for the test build (excluding freq.c)
TEST_SRCS = freq.test.c freq.internal.c hash.c eprintf.c list.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Headers
HDRS = freq.internal.h hash.h list.h eprintf.h

# Target executables
TARGET = freq
TEST_TARGET = freq_test

# Default target (normal build)
all: $(TARGET)

# Debug mode target
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Test mode (runs the current test script)
itest: CFLAGS += $(TEST_FLAGS)
itest: $(TARGET)
	./test.sh

# Unit test mode (compiles and runs unit tests)
utest: CFLAGS += $(TEST_FLAGS)
utest: $(TEST_TARGET)
	./$(TEST_TARGET)

# Rule to build the target
$(TARGET): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

# Rule to build the unit test target
$(TEST_TARGET): $(TEST_OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_TARGET) $(TEST_OBJS)

# Clean target to remove object files and executables
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)

# Phony targets
.PHONY: all debug release clean itest utest

