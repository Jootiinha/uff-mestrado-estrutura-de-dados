#include "../libraries/timer.h"
#include <iostream>
#include <vector>

#include "../libraries/bubble_sort.h"

using namespace std;

int main() {
  std::vector<int> lista = {28, 22, 10, 9, 3, 1, 8, 4, 7, 6};

  Timer t;
  BubbleSort bubbleSort;

  bubbleSort.sort(lista);

  t.printElapsed();

  cout << "Lista ordenada: ";
  for (int x : lista)
    cout << x << " ";
  cout << endl;

  return 0;
}