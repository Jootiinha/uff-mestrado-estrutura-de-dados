#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <ctime>
#include <vector>

class Utils {
public:
  Utils() { srand(time(NULL)); }

  Utils(unsigned int seed) { srand(seed); }

  void setSeed(unsigned int seed) { srand(seed); }

  void preencherVetor(std::vector<int> &v, int n, int max_val = 100) {
    v.clear();
    v.reserve(n);
    for (int i = 0; i < n; i++) {
      v.push_back(rand() % (max_val + 1));
    }
  }

  void preencherVetor(int *v, int n, int max_val = 100) {
    for (int i = 0; i < n; i++) {
      v[i] = rand() % (max_val + 1);
    }
  }
};

#endif
