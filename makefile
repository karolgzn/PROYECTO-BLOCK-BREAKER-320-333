SRC_DIR := src
BIN_DIR := bin
INC_DIR := include

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -pipe

# Prefer pkg-config for SFML if available (works well under MSYS2/MinGW)
ifeq ($(shell pkg-config --exists sfml-all >/dev/null 2>&1 && echo yes),yes)
	CXXFLAGS += $(shell pkg-config --cflags sfml-all)
	LIBS := $(shell pkg-config --libs sfml-all)
else
	# If you installed SFML manually for MinGW-w64, set SFML_DIR accordingly
	# Example for MSYS2 mingw64 environment: set SFML_DIR=/mingw64
	SFML_DIR ?= /mingw64
	CXXFLAGS += -I$(SFML_DIR)/include
	LIBS := -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
endif

export TMP := $(CURDIR)/tmp
export TEMP := $(CURDIR)/tmp
export TMPDIR := $(CURDIR)/tmp

TARGET := $(BIN_DIR)/arkanoid.exe
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

all: tmp $(BIN_DIR) $(TARGET)

tmp:
	mkdir -p tmp

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	@echo "Ejecutando Arkanoid..."
	@cd $(CURDIR) && ./$(TARGET)

clean:
	rm -rf $(BIN_DIR)/*.o $(TARGET)

distclean:
	rm -rf $(BIN_DIR) tmp

.PHONY: all run clean distclean tmp
