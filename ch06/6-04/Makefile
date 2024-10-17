# Compiler and flags for normal, debug, release, and edebug modes
CC = gcc
CFLAGS = -Wall -Wextra
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2

# Include directories (if your headers are in the same directory)
INCLUDES = -I.

# Headers (all headers, including test-specific ones)
HDRS = freq.internal.h hash.h list.h eprintf.h jankunit.h shmalloc.h


############################### SOURCES ####################################### 
# Source files that are constant across targets
CONST_SRC = freq.internal.c eprintf.c shmalloc.c hash.c list.c

# Source files for the main build
MAIN_SRCS = freq.main.c $(CONST_SRC)

# Source files for unit tests (including test-specific sources)
TEST_SRCS = freq.test.c jankunit.c shmalloc.c $(CONST_SRC)

# Expanded sources for edebug
EXPANDED_MAIN_SRCS = $(MAIN_SRCS:.c=.expanded.c)
EXPANDED_TEST_SRCS = $(TEST_SRCS:.c=.expanded.c)

###############################################################################


############################### OBJECTS ####################################### 
# Object files for the normal build
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

# Object files for the test build
TEST_OBJS = $(TEST_SRCS:.c=.o)

###############################################################################


############################### TARGETS #######################################
# Target executables
MAIN_TARGET = freq
TEST_TARGET = freq_test

# Default target (builds the main executable)
all: $(MAIN_TARGET)

# Debug mode target
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(MAIN_TARGET)

# Edebug mode for the main target
edebug: CFLAGS += $(DEBUG_FLAGS)
edebug: $(EXPANDED_MAIN_SRCS) $(MAIN_TARGET)

# Edebug mode for unit tests (eutest)
eutest: CFLAGS += $(DEBUG_FLAGS)
eutest: $(EXPANDED_TEST_SRCS) $(TEST_TARGET)

# Release mode target
release: CFLAGS += $(RELEASE_FLAGS)
release: $(MAIN_TARGET)

###############################################################################


# Pattern rule to preprocess .c files to their expanded versions
%.expanded.c: %.c $(HDRS)
	$(CC) -E $(INCLUDES) $< -o $@

# Pattern rule to compile expanded sources into object files
%.o: %.expanded.c $(HDRS)
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

# Clean target to remove object files, executables, and expanded sources
clean:
	rm -f $(MAIN_OBJS) $(TEST_OBJS) $(MAIN_TARGET) $(TEST_TARGET) \
	      $(EXPANDED_MAIN_SRCS) $(EXPANDED_TEST_SRCS)

# Phony targets
.PHONY: all debug release clean itest utest edebug eutest

