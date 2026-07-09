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

double average_runtime(BlockSortMode mode,
                       const vector<vector<int>> &benchmark_samples,
                       const BlockSortCombination &combination,
                       uint32_t partition_seed_base) {
  using Clock = chrono::high_resolution_clock;

  double total_seconds = 0.0;

  for (size_t sample_index = 0; sample_index < benchmark_samples.size();
       ++sample_index) {
    const uint32_t partition_seed =
        partition_seed_base + static_cast<uint32_t>(sample_index);
    const auto start_time = Clock::now();
    const BlockSortExecution execution =
        BlockSort::run_mode(mode, benchmark_samples[sample_index], combination,
                            partition_seed);
    const auto end_time = Clock::now();

    if (!BlockSort::are_valid_block_sizes(execution.blocks_before_sort) ||
        !BlockSort::is_sorted_non_decreasing(execution.result) ||
        !BlockSort::preserves_elements(benchmark_samples[sample_index],
                                       execution.result)) {
      throw logic_error(string("Invalid result while benchmarking mode: ") +
                        BlockSort::mode_name(mode));
    }

    total_seconds += chrono::duration<double>(end_time - start_time).count();
  }

  return total_seconds / static_cast<double>(benchmark_samples.size());
}

void write_results_csv(const vector<BenchmarkRow> &rows,
                       const filesystem::path &path) {
  ofstream output_file(path);
  output_file
      << "size,sequential_seconds,threads_seconds,openmp_seconds,best_combination\n";

  for (const auto &row : rows) {
    output_file << row.size << ',' << fixed << setprecision(8)
                << row.sequential_seconds << ',' << row.threads_seconds << ','
                << row.openmp_seconds << ",\""
                << BlockSort::combination_name(row.combination) << "\"\n";
  }
}

void write_results_dat(const vector<BenchmarkRow> &rows,
                       const filesystem::path &path) {
  ofstream output_file(path);
  output_file << "# size sequential threads openmp\n";

  for (const auto &row : rows) {
    output_file << row.size << ' ' << fixed << setprecision(8)
                << row.sequential_seconds << ' ' << row.threads_seconds << ' '
                << row.openmp_seconds << '\n';
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
  cout << "OpenMP: habilitado em compilacao." << '\n';

  for (size_t size_index = 0; size_index < sizes.size(); ++size_index) {
    const int size = sizes[size_index];
    const uint32_t input_seed = input_seed_base + static_cast<uint32_t>(size_index);
    const uint32_t partition_seed =
        partition_seed_base + static_cast<uint32_t>(size_index * 1000);

    cout << "Tamanho " << size << ": gerando " << iterations
         << " vetores base..." << '\n';

    const auto benchmark_samples =
        BlockSort::make_benchmark_inputs(size, iterations, max_value, input_seed);
    const auto combination_selection_samples = BlockSort::make_benchmark_inputs(
        selection_size, selection_iterations, max_value, input_seed + 500);

    cout << "Tamanho " << size << ": escolhendo melhor combinacao..." << '\n';
    const BlockSortBenchmarkSummary best_combination_summary =
        BlockSort::select_best_combination(combination_selection_samples,
                                           partition_seed);

    cout << "  Melhor combinacao: "
         << BlockSort::combination_name(best_combination_summary.combination)
         << '\n';
    cout << "  Tempo medio da selecao: "
         << best_combination_summary.average_seconds << " s" << '\n';

    BenchmarkRow benchmark_row;
    benchmark_row.size = size;
    benchmark_row.combination = best_combination_summary.combination;

    benchmark_row.sequential_seconds =
        average_runtime(BlockSortMode::Sequential, benchmark_samples,
                        benchmark_row.combination, partition_seed);
    benchmark_row.threads_seconds =
        average_runtime(BlockSortMode::Threads, benchmark_samples,
                        benchmark_row.combination, partition_seed);
    benchmark_row.openmp_seconds =
        average_runtime(BlockSortMode::OpenMP, benchmark_samples,
                        benchmark_row.combination, partition_seed);

    cout << "  Medias:" << '\n';
    cout << "  - sequencial: " << benchmark_row.sequential_seconds << " s"
         << '\n';
    cout << "  - std::thread: " << benchmark_row.threads_seconds << " s"
         << '\n';
    cout << "  - openmp: " << benchmark_row.openmp_seconds << " s" << '\n';

    rows.push_back(benchmark_row);
    write_results_csv(rows, "results/benchmark_results.csv");
    write_results_dat(rows, "results/averages.dat");
  }

  cout << "Benchmark concluido." << '\n';
  cout << "Arquivos gerados:" << '\n';
  cout << "- results/benchmark_results.csv" << '\n';
  cout << "- results/averages.dat" << '\n';

  return 0;
}
