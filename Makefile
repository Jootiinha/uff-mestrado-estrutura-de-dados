CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ilibraries
UNAME_S := $(shell uname -s)
LIBOMP_PREFIX := $(shell brew --prefix libomp 2>/dev/null)
DEFAULT_OPENMP_FLAGS = -fopenmp

ifeq ($(UNAME_S),Darwin)
  DEFAULT_OPENMP_FLAGS = -Xpreprocessor -fopenmp -I$(LIBOMP_PREFIX)/include -L$(LIBOMP_PREFIX)/lib -lomp
endif

OPENMP_FLAGS ?= $(DEFAULT_OPENMP_FLAGS)
BIN_DIR = execs

.PHONY: all run benchmark plot clean

all: run

run: clean run.cpp
	$(CXX) $(CXXFLAGS) $(OPENMP_FLAGS) run.cpp -o run

benchmark: clean benchmark.cpp
	$(CXX) $(CXXFLAGS) $(OPENMP_FLAGS) benchmark.cpp -o benchmark
	mkdir -p results
	./benchmark

plot:
	gnuplot plot_results.gp

clean:
	rm -f run benchmark
	rm -rf $(BIN_DIR)
	rm -rf results
