#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include <vector>
#include <iostream>

using namespace std;

class QuickSort {
public:
    QuickSort() {}
    ~QuickSort() {}

    int partition(std::vector<int> &arr, int low, int high) {
        int pivot = arr[high];
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return (i + 1);
    }

    void sortRecursive(std::vector<int> &arr, int low, int high) {
        if (low < high) {
            int pi = partition(arr, low, high);

            sortRecursive(arr, low, pi - 1);
            sortRecursive(arr, pi + 1, high);
        }
    }

    void sort(std::vector<int> &arr) {
        if (!arr.empty()) {
            sortRecursive(arr, 0, arr.size() - 1);
        }
    }
};

#endif // QUICK_SORT_H
