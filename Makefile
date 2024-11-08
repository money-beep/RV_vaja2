
C = gcc

CFLAGS = -Wall -Wextra -Iinclude -g

# source and test file + all headers
MAIN_SOURCES = src/bitStack.c src/main.c src/bmp_comp_binary.c src/predict_value.c src/compress.c $(wildcard include/*.c)

# object files
MAIN_OBJECTS = $(MAIN_SOURCES:.c=.o)

MAIN_EXECUTABLE = main

all: $(MAIN_EXECUTABLE)

# rule to link object files into the main executable
$(MAIN_EXECUTABLE): $(MAIN_OBJECTS)
	$(C) $(CFLAGS) -o $@ $^ -lm

# compile source files into object files
%.o: %.cpp
	$(C) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(MAIN_OBJECTS) $(MAIN_EXECUTABLE)

.PHONY: all clean

