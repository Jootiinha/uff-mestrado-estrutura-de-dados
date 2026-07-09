#include <iostream>
#include <vector>

#include "libraries/block_sort.hpp"
#include "libraries/timer.h"
#include "libraries/utils.h"

using namespace std;

void print_vector(const vector<int> &values) {
  if (values.size() > 40) {
    cout << "Lista muito grande para exibir (tamanho: " << values.size() << ")"
         << '\n';
    return;
  }
  for (int value : values)
    cout << value << " ";
  cout << '\n';
}

void print_blocks(const vector<vector<int>> &blocks) {
  if (blocks.size() > 10) {
    cout << "Blocos demais para exibir (" << blocks.size() << " blocos)" << '\n';
    return;
  }

  for (size_t block_index = 0; block_index < blocks.size(); ++block_index) {
    cout << "Bloco " << block_index + 1 << " ("
         << blocks[block_index].size() << "): ";
    print_vector(blocks[block_index]);
  }
}

void print_ranking(const vector<BlockSortBenchmarkSummary> &ranking) {
  for (size_t ranking_index = 0; ranking_index < ranking.size();
       ++ranking_index) {
    cout << ranking_index + 1 << ". "
         << BlockSort::combination_name(ranking[ranking_index].combination)
         << " -> " << ranking[ranking_index].average_seconds << " s" << '\n';
  }
}

void print_execution_summary(const char *execution_label,
                             const BlockSortExecution &execution,
                             const vector<int> &original_values) {
  const bool has_valid_block_sizes =
      BlockSort::are_valid_block_sizes(execution.blocks_before_sort);
  const bool result_is_sorted =
      BlockSort::is_sorted_non_decreasing(execution.result);
  const bool preserves_original_elements =
      BlockSort::preserves_elements(original_values, execution.result);

  cout << execution_label << '\n';
  cout << "Blocos antes da ordenacao:" << '\n';
  print_blocks(execution.blocks_before_sort);

  cout << "Blocos depois da ordenacao:" << '\n';
  print_blocks(execution.blocks_after_sort);

  cout << "Lista final ordenada: ";
  print_vector(execution.result);

  cout << "Validacoes:" << '\n';
  cout << "- blocos 4/5: "
       << (has_valid_block_sizes ? "OK" : "FALHOU") << '\n';
  cout << "- resultado ordenado: "
       << (result_is_sorted ? "OK" : "FALHOU") << '\n';
  cout << "- elementos preservados: "
       << (preserves_original_elements ? "OK" : "FALHOU") << '\n';
}

int main(int argc, char *argv[]) {
  int size = 20;
  if (argc > 3) {
    cout << "Uso: " << argv[0] << " [tamanho] [seed]" << '\n';
    return 1;
  }
  if (argc >= 2) {
    size = stoi(argv[1]);
  }

  uint32_t seed = 12345;
  if (argc == 3) {
    seed = static_cast<uint32_t>(stoul(argv[2]));
  }

  if (!BlockSort::can_partition_into_blocks_4_or_5(size)) {
    cout << "Tamanho invalido para blocos 4/5: " << size << '\n';
    return 1;
  }

  Utils utils;
  vector<int> values;
  utils.preencherVetor(values, size, 100);

  const auto benchmark_samples =
      BlockSort::make_benchmark_inputs(size, 7, 100, seed + 1000);
  const auto ranking =
      BlockSort::rank_combinations(benchmark_samples, seed + 2000);
  const BlockSortBenchmarkSummary best_combination_summary = ranking.front();

  cout << "Executando algoritmo sequencial por blocos com " << size
       << " elementos e seed " << seed << "." << '\n';
  cout << "OpenMP: habilitado em compilacao." << '\n';
  cout << "Combinacoes validas encontradas: " << ranking.size() << '\n';
  cout << "Melhor combinacao automatica: "
       << BlockSort::combination_name(best_combination_summary.combination)
       << '\n';
  cout << "Tempo medio no benchmark inicial: "
       << best_combination_summary.average_seconds << " s" << '\n';
  cout << "Ranking das combinacoes:" << '\n';
  print_ranking(ranking);
  cout << "Lista original: ";
  print_vector(values);

  Timer sequential_timer;
  BlockSortExecution sequential_execution =
      BlockSort::run_sequential(values, best_combination_summary.combination,
                                seed);
  const auto sequential_elapsed = sequential_timer.stop();

  Timer threads_timer;
  BlockSortExecution threads_execution =
      BlockSort::run_threads(values, best_combination_summary.combination,
                             seed);
  const auto threads_elapsed = threads_timer.stop();

  Timer openmp_timer;
  BlockSortExecution openmp_execution =
      BlockSort::run_openmp(values, best_combination_summary.combination, seed);
  const auto openmp_elapsed = openmp_timer.stop();

  const bool threads_match_sequential_partition =
      sequential_execution.blocks_before_sort ==
      threads_execution.blocks_before_sort;
  const bool threads_match_sequential_result =
      sequential_execution.result == threads_execution.result;
  const bool openmp_matches_sequential_partition =
      sequential_execution.blocks_before_sort ==
      openmp_execution.blocks_before_sort;
  const bool openmp_matches_sequential_result =
      sequential_execution.result == openmp_execution.result;

  cout << "Tempo sequencial: " << sequential_elapsed[0] << " s" << '\n';
  cout << "Tempo std::thread: " << threads_elapsed[0] << " s" << '\n';
  cout << "Tempo OpenMP: " << openmp_elapsed[0] << " s" << '\n';
  cout << "Comparacao entre versoes:" << '\n';
  cout << "- sequencial vs std::thread, mesmo particionamento: "
       << (threads_match_sequential_partition ? "OK" : "FALHOU")
       << '\n';
  cout << "- sequencial vs std::thread, mesmo resultado final: "
       << (threads_match_sequential_result ? "OK" : "FALHOU")
       << '\n';
  cout << "- sequencial vs OpenMP, mesmo particionamento: "
       << (openmp_matches_sequential_partition ? "OK" : "FALHOU")
       << '\n';
  cout << "- sequencial vs OpenMP, mesmo resultado final: "
       << (openmp_matches_sequential_result ? "OK" : "FALHOU")
       << '\n';

  print_execution_summary("Execucao sequencial:", sequential_execution, values);
  print_execution_summary("Execucao std::thread:", threads_execution, values);
  print_execution_summary("Execucao OpenMP:", openmp_execution, values);

  if (!threads_match_sequential_partition ||
      !threads_match_sequential_result ||
      !openmp_matches_sequential_partition ||
      !openmp_matches_sequential_result ||
      !BlockSort::are_valid_block_sizes(sequential_execution.blocks_before_sort) ||
      !BlockSort::is_sorted_non_decreasing(sequential_execution.result) ||
      !BlockSort::preserves_elements(values, sequential_execution.result) ||
      !BlockSort::are_valid_block_sizes(threads_execution.blocks_before_sort) ||
      !BlockSort::is_sorted_non_decreasing(threads_execution.result) ||
      !BlockSort::preserves_elements(values, threads_execution.result) ||
      !BlockSort::are_valid_block_sizes(openmp_execution.blocks_before_sort) ||
      !BlockSort::is_sorted_non_decreasing(openmp_execution.result) ||
      !BlockSort::preserves_elements(values, openmp_execution.result)) {
    return 1;
  }

  return 0;
}
