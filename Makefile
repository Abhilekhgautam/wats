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
WASM_TARGET = $(WASM_DIR)/app.js # Output JS file for WASM

# Recursively find all .cpp and .hpp files in the src directory and its subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
HDRS = $(shell find $(SRC_DIR) -name '*.hpp')

# Generate object file paths from source file paths
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
WASM_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(WASM_DIR)/%.o, $(SRCS))

# Emscripten flags
EMFLAGS = -s EXPORTED_FUNCTIONS='["_compile_program", "_malloc", "_free", "stringToUTF8"]' \
          -s EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']" \
          -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0 \
          -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=1 -s SAFE_HEAP=1 -gsource-map -s INITIAL_MEMORY=128MB

all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -g $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

wasm: $(WASM_TARGET)

# Link WASM object files and export the compile_program function
$(WASM_TARGET): $(WASM_OBJS)
	@mkdir -p $(WASM_DIR)
	$(EMXX) $(CXXFLAGS) $(EMFLAGS) -o $@ $^

# Compile source files into WASM object files
$(WASM_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(EMXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(WASM_DIR)

# Phony targets
.PHONY: all clean wasm
