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

# Test files (exclude benchmark files)
TEST_SOURCES := $(filter-out $(TESTDIR)/bench_%.cpp, $(wildcard $(TESTDIR)/*.cpp))
TEST_OBJECTS := $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/$(TESTDIR)/%.o)
TEST_EXECUTABLE := test_runner

# Benchmark files (include both bench_*.cpp and other test files except test_main.cpp)
BENCH_MAIN := $(wildcard $(TESTDIR)/bench_main.cpp)
OTHER_TEST_SOURCES := $(filter-out $(TESTDIR)/test_main.cpp, $(TEST_SOURCES))
BENCH_SOURCES := $(BENCH_MAIN) $(OTHER_TEST_SOURCES) $(filter-out $(BENCH_MAIN), $(wildcard $(TESTDIR)/bench_*.cpp))
BENCH_OBJECTS := $(BENCH_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/$(TESTDIR)/%.o)
BENCH_EXECUTABLE := benchmark_runner
BENCH_CXXFLAGS := $(CXXFLAGS) -DCATCH_CONFIG_ENABLE_BENCHMARKING -O3

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

# Compile test files to object files (test files only)
$(OBJDIR)/$(TESTDIR)/test_%.o: $(TESTDIR)/test_%.cpp | $(OBJDIR)/$(TESTDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run tests
.PHONY: test
test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

# Build benchmark runner
$(BENCH_EXECUTABLE): $(BENCH_OBJECTS) $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
	$(CXX) $(BENCH_CXXFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Benchmark build complete: $(BENCH_EXECUTABLE)"

# Compile benchmark files to object files
$(OBJDIR)/$(TESTDIR)/bench_main.o: $(TESTDIR)/bench_main.cpp | $(OBJDIR)/$(TESTDIR)
	$(CXX) $(BENCH_CXXFLAGS) -c $< -o $@

$(OBJDIR)/$(TESTDIR)/bench_%.o: $(TESTDIR)/bench_%.cpp | $(OBJDIR)/$(TESTDIR)
	$(CXX) $(BENCH_CXXFLAGS) -c $< -o $@

# Run benchmarks
.PHONY: benchmark
benchmark: $(BENCH_EXECUTABLE)
	./$(BENCH_EXECUTABLE) -r console -d yes

# Clean object files and executable
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(EXECUTABLE) $(TEST_EXECUTABLE) $(BENCH_EXECUTABLE)
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
	@echo "  benchmark - Build and run performance benchmarks"
	@echo "  clean     - Remove build artifacts and executable"
	@echo "  rebuild   - Clean and rebuild everything"
	@echo "  run       - Build and run with sample arguments"
	@echo "  help      - Display this help message"
