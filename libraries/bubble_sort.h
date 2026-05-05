#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

#include <iostream>
#include <vector>

using namespace std;

class BubbleSort {
public:
  BubbleSort() {};
  ~BubbleSort() {};

  void sort(std::vector<int> &arr) {
    int n = arr.size();
    bool swapped;

    for (int i = 0; i < n - 1; i++) {
      swapped = false;

      for (int j = 0; j < n - 1 - i; j++) {
        if (arr[j] > arr[j + 1]) {
          std::swap(arr[j], arr[j + 1]);
          swapped = true;
        }
      }

      if (!swapped) {
        break;
      }
    }
  }
};

#endif // BUBBLE_SORT_H
