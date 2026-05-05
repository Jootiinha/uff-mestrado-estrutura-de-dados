#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "libraries/bubble_sort.h"
#include "libraries/insertion_sort.h"
#include "libraries/merge_sort.h"
#include "libraries/quick_sort.h"
#include "libraries/selection_sort.h"
#include "libraries/timer.h"
#include "libraries/utils.h"

using namespace std;

void createDirectory(const string &path) {
  string command = "mkdir -p " + path;
  system(command.c_str());
}

int main() {
  Utils utils;
  vector<int> sizes;
  for (int s = 5000; s <= 20000; s += 1000) {
    sizes.push_back(s);
  }

  const int iterations = 30;
  const int max_val = 100000;

  vector<string> algos = {"bubble_sort", "insertion_sort", "merge_sort",
                          "quick_sort", "selection_sort"};

  ofstream summaryFile("results/averages.dat");
  summaryFile << "# Size bubble_sort insertion_sort merge_sort quick_sort "
                 "selection_sort"
              << endl;

  cout << "Iniciando benchmarking..." << endl;

  for (int size : sizes) {
    cout << "Testando tamanho: " << size << "..." << endl;

    vector<vector<int>> base_vectors(iterations);
    for (int i = 0; i < iterations; i++) {
      utils.preencherVetor(base_vectors[i], size, max_val);
    }

    vector<double> avg_times;

    for (const string &algo : algos) {
      cout << "  Executando " << algo << "... " << flush;

      string algo_dir = "results/" + algo;
      createDirectory(algo_dir);
      string file_path = algo_dir + "/exer_" + to_string(size) + ".txt";
      ofstream individualFile(file_path);

      double total_time = 0;

      for (int i = 0; i < iterations; i++) {
        vector<int> data = base_vectors[i];
        Timer t;

        if (algo == "bubble_sort") {
          BubbleSort().sort(data);
        } else if (algo == "insertion_sort") {
          InsertionSort().sort(data);
        } else if (algo == "merge_sort") {
          MergeSort().sort(data);
        } else if (algo == "quick_sort") {
          QuickSort().sort(data);
        } else if (algo == "selection_sort") {
          SelectionSort().sort(data);
        }

        double elapsed = t.stop()[0];
        total_time += elapsed;
        individualFile << elapsed << endl;
      }

      double avg = total_time / iterations;
      avg_times.push_back(avg);
      individualFile.close();
      cout << "Média: " << avg << "s" << endl;
    }

    summaryFile << size;
    for (double t : avg_times) {
      summaryFile << " " << t;
    }
    summaryFile << endl;
  }

  summaryFile.close();
  cout << "Benchmarking concluído! Resultados em results/" << endl;

  return 0;
}
