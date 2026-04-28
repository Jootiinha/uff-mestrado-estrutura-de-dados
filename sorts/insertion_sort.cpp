#include "../libraries/timer.h"
#include <iostream>
#include <vector>

#include "../libraries/insertion_sort.h"

using namespace std;

int main() {
  std::vector<int> lista = {28, 22, 10, 9, 3, 1, 8, 4, 7, 6};

  Timer t;
  InsertionSort insertion_sort;

  insertion_sort.sort(lista);

  t.printElapsed();

  cout << "Lista ordenada: ";
  for (int x : lista)
    cout << x << " ";
  cout << endl;

  return 0;
}