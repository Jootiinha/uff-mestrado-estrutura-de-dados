#ifndef SELECTION_SORT_H
#define SELECTION_SORT_H

#include <vector>
#include <iostream>

using namespace std;

class SelectionSort {
public:
    SelectionSort() {}
    ~SelectionSort() {}

    void sort(std::vector<int> &arr) {
        int n = arr.size();

        for (int i = 0; i < n - 1; i++) {
            int min_idx = i;
            for (int j = i + 1; j < n; j++) {
                if (arr[j] < arr[min_idx]) {
                    min_idx = j;
                }
            }

            if (min_idx != i) {
                std::swap(arr[i], arr[min_idx]);
            }
        }
    }
};

#endif // SELECTION_SORT_H
