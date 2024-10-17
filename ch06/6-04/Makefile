# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2
INCLUDES = -I.

# Headers
HDRS = freq.internal.h hash.h list.h eprintf.h jankunit.h shmalloc.h

# Source files
CONST_SRC = freq.internal.c eprintf.c shmalloc.c hash.c list.c
MAIN_SRCS = freq.main.c $(CONST_SRC)
TEST_SRCS = freq.test.c jankunit.c $(CONST_SRC)

# Remove duplicate shmalloc.c if it's already in CONST_SRC
# (Optional, depending on your actual source files)
TEST_SRCS := $(filter-out shmalloc.c,$(TEST_SRCS))

# Determine if we are in 'edebug' or 'eutest' mode
ifneq ($(filter edebug eutest,$(MAKECMDGOALS)),)
    USE_EXPANDED_SRCS := 1
endif

# Set object files based on the mode
ifeq ($(USE_EXPANDED_SRCS),1)
    # Using expanded sources
    MAIN_OBJS = $(MAIN_SRCS:.c=.expanded.o)
    TEST_OBJS = $(TEST_SRCS:.c=.expanded.o)
else
    # Using regular sources
    MAIN_OBJS = $(MAIN_SRCS:.c=.o)
    TEST_OBJS = $(TEST_SRCS:.c=.o)
endif

# Targets
MAIN_TARGET = freq
TEST_TARGET = freq_test

# Default target
all: $(MAIN_TARGET)

# Build rules
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Pattern rules
ifeq ($(USE_EXPANDED_SRCS),1)
# Generate .expanded.c files from .c files
%.expanded.c: %.c $(HDRS)
	$(CC) -E -P $(INCLUDES) $< -o $@
# Compile .expanded.o from .expanded.c
%.expanded.o: %.expanded.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
else
# Compile .o from .c
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif

# Targets
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(MAIN_TARGET)

edebug: CFLAGS += $(DEBUG_FLAGS)
edebug: $(MAIN_TARGET)

eutest: CFLAGS += $(DEBUG_FLAGS)
eutest: $(TEST_TARGET)

release: CFLAGS += $(RELEASE_FLAGS)
release: $(MAIN_TARGET)

itest: all
	./test.sh

utest: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(MAIN_OBJS) $(TEST_OBJS) $(MAIN_TARGET) $(TEST_TARGET)
	rm -f $(MAIN_SRCS:.c=.expanded.c) $(TEST_SRCS:.c=.expanded.c)
	rm -f $(MAIN_SRCS:.c=.expanded.o) $(TEST_SRCS:.c=.expanded.o)

.PHONY: all debug edebug eutest release itest utest clean

