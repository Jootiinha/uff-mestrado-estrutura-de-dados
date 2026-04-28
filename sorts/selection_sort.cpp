#include "../libraries/timer.h"
#include <iostream>
#include <vector>

#include "../libraries/selection_sort.h"
#include "../libraries/utils.h"

using namespace std;

int main() {
  std::vector<int> lista;
  Utils utils(10);

  utils.preencherVetor(lista, 10);

  Timer t;
  SelectionSort selectionSort;

  selectionSort.sort(lista);

  t.printElapsed();

  cout << "Lista ordenada: ";
  for (int x : lista)
    cout << x << " ";
  cout << endl;

  return 0;
}
