#include <vector>
#include <iostream>

using namespace std;

class InsertionSort {
    public:
        InsertionSort() {};
        ~InsertionSort() {};

        void sort(std::vector<int> &arr){
            int n = arr.size();

            for (int i = 1; i < n-1; i++){
                int key = arr[i];
                int j = i -1;

                // Enquanto esquerda maior que chave empurramos uma posição para direita
                while( j >= 0 && arr[j] > key){
                    arr[j + 1] = arr[j];
                    j = j + 1;
                }

                arr[j + 1] = key;
            }
        }
    
};