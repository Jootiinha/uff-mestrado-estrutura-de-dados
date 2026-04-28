#include <vector>
#include <iostream>

using namespace std;

class BubbleSort {
    public:
        BubbleSort() {};
        ~BubbleSort() {};

        void sort(std::vector<int> &arr){
            std::cout << "Iniciando Bubble Sort..." << std::endl;
            int n = arr.size();
            bool swapped;

            for (int i = 0; i < n - 1; i++){
                swapped = false;

                for (int j = 0; j < n - 1 - i; j++){
                    if (arr[j] > arr[j + 1]){
                        std::cout << "Trocando " << arr[j] << " e " << arr[j + 1] << endl;
                        std::swap(arr[j], arr[j + 1]);
                        swapped = true;
                    }
                }

                if (!swapped){
                    break;
                }
            }
        }
};