#ifndef INSERTION_SORT_H
#define INSERTION_SORT_H

#include <vector>
#include <iostream>

using namespace std;

class InsertionSort {
    public:
        InsertionSort() {};
        ~InsertionSort() {};

        void sort(std::vector<int> &arr){
            std::cout << "Iniciando Insertion Sort..." << std::endl;
            int n = arr.size();

            for (int i = 1; i < n; i++){
                int key = arr[i];
                int j = i - 1;

                // Enquanto esquerda maior que chave empurramos uma posição para direita
                while( j >= 0 && arr[j] > key){
                    std::cout << "Movendo " << arr[j] << " para a direita" << std::endl;
                    arr[j + 1] = arr[j];
                    j = j - 1;
                }

                arr[j + 1] = key;
            }
        }
    
};

#endif // INSERTION_SORT_H