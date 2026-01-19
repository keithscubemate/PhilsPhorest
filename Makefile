# Makefile for PhilsPhorest - Decision Forest Predictor

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I./include
LDFLAGS :=
SRCDIR := src
OBJDIR := obj
INCDIR := include
TESTDIR := tests

# Source files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EXECUTABLE := pp

# Test files
TEST_SOURCES := $(wildcard $(TESTDIR)/*.cpp)
TEST_OBJECTS := $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/$(TESTDIR)/%.o)
TEST_EXECUTABLE := test_runner

# Default target
.PHONY: all
all: $(EXECUTABLE)

# Link executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Build complete: $(EXECUTABLE)"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Create test object directory if it doesn't exist
$(OBJDIR)/$(TESTDIR):
	mkdir -p $(OBJDIR)/$(TESTDIR)

# Build test runner (exclude main.o from linking)
$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Test build complete: $(TEST_EXECUTABLE)"

# Compile test files to object files
$(OBJDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)/$(TESTDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
.PHONY: test
test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

# Clean object files and executable
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(EXECUTABLE) $(TEST_EXECUTABLE)
	@echo "Clean complete"

# Rebuild everything
.PHONY: rebuild
rebuild: clean all

# Run the executable with sample arguments
.PHONY: run
run: $(EXECUTABLE)
	./$(EXECUTABLE) model.json data.csv

# Display help
.PHONY: help
help:
	@echo "PhilsPhorest Makefile targets:"
	@echo "  all       - Build the executable (default)"
	@echo "  test      - Build and run unit tests"
	@echo "  clean     - Remove build artifacts and executable"
	@echo "  rebuild   - Clean and rebuild everything"
	@echo "  run       - Build and run with sample arguments"
	@echo "  help      - Display this help message"
