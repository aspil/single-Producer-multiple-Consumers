TARGET_EXEC ?= feeder

CC = g++
CC_FLAGS = -Wall -Wextra -lrt -std=c++11

# Project directories. #
SRC_DIR ?= src
BUILD_DIR ?= build
BIN_DIR ?= bin

# Building fundamentals
SOURCES := $(shell find $(SRC_DIR) -type f -name *.cpp)
OBJECTS :=$(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SOURCES:.cpp=.o))

# Include files
INC := -I include

# Main and object targets
$(BIN_DIR)/$(TARGET_EXEC): $(OBJECTS)
	@mkdir -p bin
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $(INC) -c -o $@ $<

.PHONY:
	clean
# Removes build and bin directories
clean:
	rm -rf  $(BUILD_DIR) $(BIN_DIR) feeder