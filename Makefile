CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ilibraries
BIN_DIR = execs
ALGS = bubble_sort insertion_sort merge_sort quick_sort selection_sort

# Este bloco permite passar argumentos para o make (ex: make merge_sort 1000)
# Ele filtra o primeiro objetivo (o algoritmo) e trata o resto como argumentos.
ifeq ($(filter $(firstword $(MAKECMDGOALS)),$(ALGS)),$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

.PHONY: all clean $(ALGS)

all: run

run: clean run.cpp
	$(CXX) $(CXXFLAGS) run.cpp -o run

benchmark: clean benchmark.cpp
	$(CXX) $(CXXFLAGS) benchmark.cpp -o benchmark
	mkdir -p results
	./benchmark

clean:
	rm -f run benchmark
	rm -rf $(BIN_DIR)