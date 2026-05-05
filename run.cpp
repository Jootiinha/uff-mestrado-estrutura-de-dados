#include <iostream>
#include <string>
#include <vector>

#include "libraries/bubble_sort.h"
#include "libraries/insertion_sort.h"
#include "libraries/merge_sort.h"
#include "libraries/quick_sort.h"
#include "libraries/selection_sort.h"
#include "libraries/timer.h"
#include "libraries/utils.h"

using namespace std;

void printVector(const vector<int> &v) {
  if (v.size() > 20) {
    cout << "Lista muito grande para exibir (tamanho: " << v.size() << ")"
         << endl;
    return;
  }
  for (int x : v)
    cout << x << " ";
  cout << endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Uso: " << argv[0] << " <algoritmo> [tamanho]" << endl;
    cout << "Algoritmos: bubble_sort, insertion_sort, merge_sort, quick_sort, "
            "selection_sort"
         << endl;
    return 1;
  }

  string algo = argv[1];
  int size = 10;
  if (argc >= 3) {
    size = stoi(argv[2]);
  }

  Utils utils;
  vector<int> data;
  utils.preencherVetor(data, size);

  cout << "Executando " << algo << " com " << size << " elementos." << endl;
  cout << "Lista original: ";
  printVector(data);

  Timer t;

  if (algo == "bubble_sort") {
    BubbleSort bs;
    bs.sort(data);
  } else if (algo == "insertion_sort") {
    InsertionSort is;
    is.sort(data);
  } else if (algo == "merge_sort") {
    MergeSort ms;
    ms.sort(data);
  } else if (algo == "quick_sort") {
    QuickSort qs;
    qs.sort(data);
  } else if (algo == "selection_sort") {
    SelectionSort ss;
    ss.sort(data);
  } else {
    cout << "Algoritmo desconhecido: " << algo << endl;
    return 1;
  }

  cout << "Tempo decorrido: ";
  t.printElapsed();

  cout << "Lista ordenada: ";
  printVector(data);

  return 0;
}
