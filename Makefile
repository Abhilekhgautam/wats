# Compiler and flags
CXX = clang++ -g
EMXX = em++ -g
CXXFLAGS = -std=c++20 -Wall -Wextra

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
WASM_DIR = wasm

TARGET = $(BIN_DIR)/app
WASM_TARGET = $(WASM_DIR)/app.wasm

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
HDRS = $(wildcard $(SRC_DIR)/*.hpp)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
WASM_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(WASM_DIR)/%.o, $(SRCS))

all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -g $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

wasm: $(WASM_TARGET)

$(WASM_TARGET): $(WASM_OBJS)
	@mkdir -p $(WASM_DIR)
	$(EMXX) -o $@ $^

$(WASM_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(WASM_DIR)
	$(EMXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(WASM_DIR)

# Phony targets
.PHONY: all clean wasm
