#include <array>
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
  vector<BlockSortBenchmarkSummary> selection_ranking;
  array<double, 5> top_ranking_seconds{};
  double selection_total_seconds = 0.0;
  double selected_training_seconds = 0.0;
  double selected_validation_seconds = 0.0;
  double baseline_validation_seconds = 0.0;
  double selection_gain_percent = 0.0;
  double sequential_seconds = 0.0;
  double threads_seconds = 0.0;
  double openmp_seconds = 0.0;
};

int algorithm_code(BlockSortAlgorithm algorithm) {
  switch (algorithm) {
  case BlockSortAlgorithm::InsertionSort:
    return 1;
  case BlockSortAlgorithm::SelectionSort:
    return 2;
  case BlockSortAlgorithm::BubbleSort:
    return 3;
  case BlockSortAlgorithm::MergeSort:
    return 4;
  case BlockSortAlgorithm::QuickSort:
    return 5;
  }

  throw logic_error("Unknown block sort algorithm.");
}

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
                       const BlockSortCombination &baseline_combination,
                       const filesystem::path &path) {
  ofstream output_file(path);
  output_file
      << "size,selection_total_seconds,selected_training_seconds,"
         "selected_validation_seconds,baseline_validation_seconds,"
         "selection_gain_percent,sequential_seconds,threads_seconds,"
         "openmp_seconds,best_combination,baseline_combination\n";

  for (const auto &row : rows) {
    output_file << row.size << ',' << fixed << setprecision(8)
                << row.selection_total_seconds << ','
                << row.selected_training_seconds << ','
                << row.selected_validation_seconds << ','
                << row.baseline_validation_seconds << ','
                << row.selection_gain_percent << ',' << row.sequential_seconds
                << ',' << row.threads_seconds << ',' << row.openmp_seconds
                << ",\"" << BlockSort::combination_name(row.combination)
                << "\",\"" << BlockSort::combination_name(baseline_combination)
                << "\"\n";
  }
}

void write_selection_ranking_csv(const vector<BenchmarkRow> &rows,
                                 const filesystem::path &path) {
  ofstream output_file(path);
  output_file << "size,rank,average_seconds,combination\n";

  for (const auto &row : rows) {
    for (size_t ranking_index = 0;
         ranking_index < row.selection_ranking.size(); ++ranking_index) {
      const auto &summary = row.selection_ranking[ranking_index];
      output_file << row.size << ',' << ranking_index + 1 << ',' << fixed
                  << setprecision(8) << summary.average_seconds << ",\""
                  << BlockSort::combination_name(summary.combination)
                  << "\"\n";
    }
  }
}

void write_results_dat(const vector<BenchmarkRow> &rows,
                       const filesystem::path &path) {
  ofstream output_file(path);
  output_file
      << "# size selection_total selected_training selected_validation "
         "baseline_validation gain_percent sequential threads openmp "
         "rank_1 rank_2 rank_3 rank_4 rank_5 even_algorithm odd_algorithm "
         "final_algorithm\n";

  for (const auto &row : rows) {
    output_file << row.size << ' ' << fixed << setprecision(8)
                << row.selection_total_seconds << ' '
                << row.selected_training_seconds << ' '
                << row.selected_validation_seconds << ' '
                << row.baseline_validation_seconds << ' '
                << row.selection_gain_percent << ' ' << row.sequential_seconds
                << ' ' << row.threads_seconds << ' ' << row.openmp_seconds;

    for (const double ranking_seconds : row.top_ranking_seconds) {
      output_file << ' ' << ranking_seconds;
    }

    output_file << ' ' << algorithm_code(row.combination.even_block) << ' '
                << algorithm_code(row.combination.odd_block) << ' '
                << algorithm_code(row.combination.final_stage) << '\n';
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
  const BlockSortCombination baseline_combination;

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
    const auto combination_validation_samples =
        BlockSort::make_benchmark_inputs(selection_size, selection_iterations,
                                         max_value, input_seed + 1500);

    cout << "Tamanho " << size << ": escolhendo melhor combinacao..." << '\n';
    const auto selection_start_time = chrono::high_resolution_clock::now();
    const vector<BlockSortBenchmarkSummary> selection_ranking =
        BlockSort::rank_combinations(combination_selection_samples,
                                     partition_seed);
    const auto selection_end_time = chrono::high_resolution_clock::now();

    if (selection_ranking.size() < 5) {
      throw logic_error("Fewer than five valid combinations in the ranking.");
    }

    const BlockSortBenchmarkSummary &best_combination_summary =
        selection_ranking.front();

    cout << "  Melhor combinacao: "
         << BlockSort::combination_name(best_combination_summary.combination)
         << '\n';
    cout << "  Tempo total da selecao: "
         << chrono::duration<double>(selection_end_time - selection_start_time)
                .count()
         << " s" << '\n';
    cout << "  Tempo medio da vencedora na amostra de selecao: "
         << best_combination_summary.average_seconds << " s" << '\n';

    BenchmarkRow benchmark_row;
    benchmark_row.size = size;
    benchmark_row.combination = best_combination_summary.combination;
    benchmark_row.selection_ranking = selection_ranking;
    benchmark_row.selection_total_seconds =
        chrono::duration<double>(selection_end_time - selection_start_time)
            .count();
    benchmark_row.selected_training_seconds =
        best_combination_summary.average_seconds;

    for (size_t ranking_index = 0;
         ranking_index < benchmark_row.top_ranking_seconds.size();
         ++ranking_index) {
      benchmark_row.top_ranking_seconds[ranking_index] =
          selection_ranking[ranking_index].average_seconds;
    }

    benchmark_row.selected_validation_seconds = average_runtime(
        BlockSortMode::Sequential, combination_validation_samples,
        benchmark_row.combination, partition_seed + 500000);
    benchmark_row.baseline_validation_seconds = average_runtime(
        BlockSortMode::Sequential, combination_validation_samples,
        baseline_combination, partition_seed + 500000);

    if (benchmark_row.baseline_validation_seconds > 0.0) {
      benchmark_row.selection_gain_percent =
          (benchmark_row.baseline_validation_seconds -
           benchmark_row.selected_validation_seconds) /
          benchmark_row.baseline_validation_seconds * 100.0;
    }

    cout << "  Validacao independente:" << '\n';
    cout << "  - combinacao selecionada: "
         << benchmark_row.selected_validation_seconds << " s" << '\n';
    cout << "  - combinacao fixa: "
         << benchmark_row.baseline_validation_seconds << " s" << '\n';
    cout << "  - ganho da selecao: " << benchmark_row.selection_gain_percent
         << "%" << '\n';

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
    write_results_csv(rows, baseline_combination,
                      "results/benchmark_results.csv");
    write_selection_ranking_csv(rows, "results/selection_ranking.csv");
    write_results_dat(rows, "results/averages.dat");
  }

  cout << "Benchmark concluido." << '\n';
  cout << "Arquivos gerados:" << '\n';
  cout << "- results/benchmark_results.csv" << '\n';
  cout << "- results/selection_ranking.csv" << '\n';
  cout << "- results/averages.dat" << '\n';

  return 0;
}
