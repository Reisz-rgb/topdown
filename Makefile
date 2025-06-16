# Makefile for Raylib Project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -O2

# Raylib configuration
RAYLIB_PATH = /workspaces/raylib
RAYLIB_INCLUDE = $(RAYLIB_PATH)/src
RAYLIB_LIB = $(RAYLIB_PATH)/src

# Executable name
TARGET = game

# Source files (change this to match your project)
SOURCES = main.c resources.c title_screen.c main_menu.c options_menu.c game_screen.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Linker flags
LDFLAGS = -L$(RAYLIB_LIB) -lraylib -lm -ldl -lrt -lX11 -lpthread

# Default target
all: $(TARGET)

# Linking the target executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -I$(RAYLIB_INCLUDE) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Rebuild
rebuild: clean all

# Run the game
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean rebuild run