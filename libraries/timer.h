#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>

class Timer {
public:
  Timer() { start(); }

  void start() { startTime = std::chrono::high_resolution_clock::now(); }

  double stop() {
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    return elapsed.count();
  }

  void printElapsed() {
    double seconds = stop();
    std::cout << "Tempo de execução: " << std::fixed << std::setprecision(8)
              << seconds << " segundos." << std::endl;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

#endif // TIMER_H
