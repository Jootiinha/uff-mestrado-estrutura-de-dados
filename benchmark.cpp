#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "libraries/block_sort.hpp"

using namespace std;

struct BenchmarkRow {
  int size = 0;
  BlockSortCombination combination;
  double sequential_seconds = 0.0;
  double threads_seconds = 0.0;
  double openmp_seconds = 0.0;
};

double average_runtime(BlockSortMode mode, const vector<vector<int>> &inputs,
                       const BlockSortCombination &combination,
                       uint32_t partition_seed_base) {
  using Clock = chrono::high_resolution_clock;

  double total_seconds = 0.0;

  for (size_t i = 0; i < inputs.size(); ++i) {
    const uint32_t seed = partition_seed_base + static_cast<uint32_t>(i);
    const auto start = Clock::now();
    const BlockSortExecution execution =
        BlockSort::run_mode(mode, inputs[i], combination, seed);
    const auto end = Clock::now();

    if (!BlockSort::are_valid_block_sizes(execution.blocks_before_sort) ||
        !BlockSort::is_sorted_non_decreasing(execution.result) ||
        !BlockSort::preserves_elements(inputs[i], execution.result)) {
      throw logic_error(string("Invalid result while benchmarking mode: ") +
                        BlockSort::mode_name(mode));
    }

    total_seconds += chrono::duration<double>(end - start).count();
  }

  return total_seconds / static_cast<double>(inputs.size());
}

void write_results_csv(const vector<BenchmarkRow> &rows,
                       const filesystem::path &path) {
  ofstream out(path);
  out << "size,sequential_seconds,threads_seconds,openmp_seconds,best_combination\n";

  for (const auto &row : rows) {
    out << row.size << ',' << fixed << setprecision(8) << row.sequential_seconds
        << ',' << row.threads_seconds << ',' << row.openmp_seconds << ",\""
        << BlockSort::combination_name(row.combination) << "\"\n";
  }
}

void write_results_dat(const vector<BenchmarkRow> &rows,
                       const filesystem::path &path) {
  ofstream out(path);
  out << "# size sequential threads openmp\n";

  for (const auto &row : rows) {
    out << row.size << ' ' << fixed << setprecision(8) << row.sequential_seconds
        << ' ' << row.threads_seconds << ' ' << row.openmp_seconds << '\n';
  }
}

int main() {
  const vector<int> sizes = {15000, 16000, 17000, 18000, 19000, 20000};
  const int iterations = 30;
  const int selection_iterations = 5;
  const int selection_size = 2000;
  const int max_value = 100000;
  const uint32_t input_seed_base = 20260709;
  const uint32_t partition_seed_base = 91000;

  filesystem::create_directories("results");

  vector<BenchmarkRow> rows;
  rows.reserve(sizes.size());

  cout << "Iniciando benchmark oficial..." << '\n';
#ifdef _OPENMP
  cout << "OpenMP: habilitado em compilacao." << '\n';
#else
  cout << "OpenMP: indisponivel neste ambiente; coluna openmp refletira o "
          "fallback sequencial."
       << '\n';
#endif

  for (size_t size_index = 0; size_index < sizes.size(); ++size_index) {
    const int size = sizes[size_index];
    const uint32_t input_seed = input_seed_base + static_cast<uint32_t>(size_index);
    const uint32_t partition_seed =
        partition_seed_base + static_cast<uint32_t>(size_index * 1000);

    cout << "Tamanho " << size << ": gerando " << iterations
         << " vetores base..." << '\n';

    const auto inputs =
        BlockSort::make_benchmark_inputs(size, iterations, max_value, input_seed);
    const auto selection_inputs = BlockSort::make_benchmark_inputs(
        selection_size, selection_iterations, max_value, input_seed + 500);

    cout << "Tamanho " << size << ": escolhendo melhor combinacao..." << '\n';
    const BlockSortBenchmarkSummary best =
        BlockSort::select_best_combination(selection_inputs, partition_seed);

    cout << "  Melhor combinacao: "
         << BlockSort::combination_name(best.combination) << '\n';
    cout << "  Tempo medio da selecao: " << best.average_seconds << " s" << '\n';

    BenchmarkRow row;
    row.size = size;
    row.combination = best.combination;

    row.sequential_seconds = average_runtime(BlockSortMode::Sequential, inputs,
                                             row.combination, partition_seed);
    row.threads_seconds = average_runtime(BlockSortMode::Threads, inputs,
                                          row.combination, partition_seed);
    row.openmp_seconds = average_runtime(BlockSortMode::OpenMP, inputs,
                                         row.combination, partition_seed);

    cout << "  Medias:" << '\n';
    cout << "  - sequencial: " << row.sequential_seconds << " s" << '\n';
    cout << "  - std::thread: " << row.threads_seconds << " s" << '\n';
    cout << "  - openmp: " << row.openmp_seconds << " s" << '\n';

    rows.push_back(row);
    write_results_csv(rows, "results/benchmark_results.csv");
    write_results_dat(rows, "results/averages.dat");
  }

  cout << "Benchmark concluido." << '\n';
  cout << "Arquivos gerados:" << '\n';
  cout << "- results/benchmark_results.csv" << '\n';
  cout << "- results/averages.dat" << '\n';

  return 0;
}
