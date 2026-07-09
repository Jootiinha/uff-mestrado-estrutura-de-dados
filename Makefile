CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ilibraries
OPENMP_FLAGS ?=
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
