CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS =

# Define source files
SRCS = main.cpp \
       src/compiler/lexer/lexer.cpp \
       src/compiler/parser/parser.cpp \
       src/compiler/codegen/compiler.cpp \
       src/compiler/codegen/bytecode.cpp \
       src/compiler/codegen/vm.cpp

# Define object files
OBJS = $(SRCS:.cpp=.o)

# Define output executable
TARGET = fusion

# Debug flags (uncomment to enable)
# CXXFLAGS += -DDEBUG_TRACE_EXECUTION -DDEBUG_PRINT_CODE

# Default target with timing
all:
	@bash -c 'start=$$(date +%s); \
	echo "Starting build..."; \
	make build; \
	end=$$(date +%s); \
	echo "Build completed in $$((end - start)) seconds"'

# Actual build steps (compilation + linking)
build: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@$(CXX) $(LDFLAGS) $^ -o $@

# Compile source files
%.o: %.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJS) $(TARGET)
	@echo "Clean complete"

# Run the example
run: $(TARGET)
	@echo "Running example..."
	@./$(TARGET) example.ll

.PHONY: all build clean run
