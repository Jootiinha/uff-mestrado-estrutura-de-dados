#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

class Timer {
public:
  Timer() { start(); }

  void start() {
    startTimeChrono = std::chrono::high_resolution_clock::now();
    startTimeWithoutChrono = clock();
  }

  std::vector<double> stop() {


    std::chrono::time_point<std::chrono::high_resolution_clock> endTimeChrono = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedChrono = endTimeChrono - startTimeChrono;

    double durationChrono = elapsedChrono.count();

    double clockResult = (double)(std::clock() - startTimeWithoutChrono) / CLOCKS_PER_SEC;

    return {
      durationChrono,
      clockResult
    };
  }

  void printElapsed() {
    std::vector<double> seconds = stop();

    std::cout << "Tempo de execução: " << std::fixed << std::setprecision(8)
              << seconds[0] << " segundos." << std::endl;

    std::cout << "Tempo de CPU: " << std::fixed << std::setprecision(8)
              << seconds[1] << " segundos." << std::endl;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> startTimeChrono;
  std::clock_t startTimeWithoutChrono;
};

#endif // TIMER_H
