CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ilibraries
SRC_DIR = sorts
BIN_DIR = execs

# List of source files (basenames)
SRCS_BASE = bubble_sort insertion_sort merge_sort quick_sort selection_sort
# List of executables
BINS = $(SRCS_BASE:%=$(BIN_DIR)/%)

.PHONY: all clean run_%

all: $(BIN_DIR) $(BINS)

# Create the binary directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Pattern rule to compile .cpp from SRC_DIR to executables in BIN_DIR
$(BIN_DIR)/%: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Specific targets to run each algorithm
# Usage: make run_bubble_sort
run_%: $(BIN_DIR)/%
	./$<

clean:
	rm -rf $(BIN_DIR)
	mkdir -p $(BIN_DIR)