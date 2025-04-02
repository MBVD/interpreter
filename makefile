CXX = g++
CXXFLAGS = -std=c++23 -g
CPPFLAGS = -I$(INC_DIR) -MMD -MP -MF $(DEP_DIR)/$*.d

SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
DEP_DIR = $(BUILD_DIR)/dep
BIN_DIR = $(BUILD_DIR)/bin

SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SRCS))
TARGET = $(BIN_DIR)/main

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "Linking $^..."
	@$(CXX) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	@echo "Compiling $<..."
	@echo "creating sub directory..."
	@mkdir -p $(dir $@)
	@echo "creating sub directory..."
	@mkdir -p $(dir $(DEP_DIR)/$*)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ > logger.txt 2>&1

$(BIN_DIR) $(OBJ_DIR) $(DEP_DIR):
	@mkdir -p $@

-include $(DEPS)

run: $(TARGET)
	@echo "Running $<..."
	@$(TARGET)

debug: $(TARGET)
	@echo "Debugging $<..."
	@gdb $(TARGET)

clean:
	@echo "cleaning..."
	@rm -rf $(BUILD_DIR)
	@sleep 1
	@clear

.PHONY: all clean run debug