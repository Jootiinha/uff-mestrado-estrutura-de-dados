#include <iostream>
#include <vector>

#include "libraries/block_sort.hpp"
#include "libraries/timer.h"
#include "libraries/utils.h"

using namespace std;

void print_vector(const vector<int> &v) {
  if (v.size() > 40) {
    cout << "Lista muito grande para exibir (tamanho: " << v.size() << ")"
         << '\n';
    return;
  }
  for (int x : v)
    cout << x << " ";
  cout << '\n';
}

void print_blocks(const vector<vector<int>> &blocks) {
  if (blocks.size() > 10) {
    cout << "Blocos demais para exibir (" << blocks.size() << " blocos)" << '\n';
    return;
  }

  for (size_t i = 0; i < blocks.size(); ++i) {
    cout << "Bloco " << i + 1 << " (" << blocks[i].size() << "): ";
    print_vector(blocks[i]);
  }
}

void print_ranking(const vector<BlockSortBenchmarkSummary> &ranking) {
  for (size_t i = 0; i < ranking.size(); ++i) {
    cout << i + 1 << ". " << BlockSort::combination_name(ranking[i].combination)
         << " -> " << ranking[i].average_seconds << " s" << '\n';
  }
}

void print_execution_summary(const char *label, const BlockSortExecution &execution,
                             const vector<int> &original) {
  const bool valid_blocks =
      BlockSort::are_valid_block_sizes(execution.blocks_before_sort);
  const bool sorted = BlockSort::is_sorted_non_decreasing(execution.result);
  const bool same_elements =
      BlockSort::preserves_elements(original, execution.result);

  cout << label << '\n';
  cout << "Blocos antes da ordenacao:" << '\n';
  print_blocks(execution.blocks_before_sort);

  cout << "Blocos depois da ordenacao:" << '\n';
  print_blocks(execution.blocks_after_sort);

  cout << "Lista final ordenada: ";
  print_vector(execution.result);

  cout << "Validacoes:" << '\n';
  cout << "- blocos 4/5: " << (valid_blocks ? "OK" : "FALHOU") << '\n';
  cout << "- resultado ordenado: " << (sorted ? "OK" : "FALHOU") << '\n';
  cout << "- elementos preservados: " << (same_elements ? "OK" : "FALHOU")
       << '\n';
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
  vector<int> data;
  utils.preencherVetor(data, size, 100);

  const auto benchmark_inputs =
      BlockSort::make_benchmark_inputs(size, 7, 100, seed + 1000);
  const auto ranking = BlockSort::rank_combinations(benchmark_inputs, seed + 2000);
  const BlockSortBenchmarkSummary best = ranking.front();

  cout << "Executando algoritmo sequencial por blocos com " << size
       << " elementos e seed " << seed << "." << '\n';
#ifdef _OPENMP
  cout << "OpenMP: habilitado em compilacao." << '\n';
#else
  cout << "OpenMP: indisponivel neste ambiente de compilacao; execucao cai no "
          "mesmo fluxo sequencial."
       << '\n';
#endif
  cout << "Combinacoes validas encontradas: " << ranking.size() << '\n';
  cout << "Melhor combinacao automatica: "
       << BlockSort::combination_name(best.combination) << '\n';
  cout << "Tempo medio no benchmark inicial: " << best.average_seconds << " s"
       << '\n';
  cout << "Ranking das combinacoes:" << '\n';
  print_ranking(ranking);
  cout << "Lista original: ";
  print_vector(data);

  Timer sequential_timer;
  BlockSortExecution sequential_execution =
      BlockSort::run_sequential(data, best.combination, seed);
  const auto sequential_elapsed = sequential_timer.stop();

  Timer threads_timer;
  BlockSortExecution threads_execution =
      BlockSort::run_threads(data, best.combination, seed);
  const auto threads_elapsed = threads_timer.stop();

  Timer openmp_timer;
  BlockSortExecution openmp_execution =
      BlockSort::run_openmp(data, best.combination, seed);
  const auto openmp_elapsed = openmp_timer.stop();

  const bool same_partition =
      sequential_execution.blocks_before_sort == threads_execution.blocks_before_sort;
  const bool same_result =
      sequential_execution.result == threads_execution.result;
  const bool same_partition_openmp =
      sequential_execution.blocks_before_sort == openmp_execution.blocks_before_sort;
  const bool same_result_openmp =
      sequential_execution.result == openmp_execution.result;

  cout << "Tempo sequencial: " << sequential_elapsed[0] << " s" << '\n';
  cout << "Tempo std::thread: " << threads_elapsed[0] << " s" << '\n';
  cout << "Tempo OpenMP: " << openmp_elapsed[0] << " s" << '\n';
  cout << "Comparacao entre versoes:" << '\n';
  cout << "- sequencial vs std::thread, mesmo particionamento: "
       << (same_partition ? "OK" : "FALHOU")
       << '\n';
  cout << "- sequencial vs std::thread, mesmo resultado final: "
       << (same_result ? "OK" : "FALHOU")
       << '\n';
  cout << "- sequencial vs OpenMP, mesmo particionamento: "
       << (same_partition_openmp ? "OK" : "FALHOU")
       << '\n';
  cout << "- sequencial vs OpenMP, mesmo resultado final: "
       << (same_result_openmp ? "OK" : "FALHOU")
       << '\n';

  print_execution_summary("Execucao sequencial:", sequential_execution, data);
  print_execution_summary("Execucao std::thread:", threads_execution, data);
  print_execution_summary("Execucao OpenMP:", openmp_execution, data);

  if (!same_partition || !same_result || !same_partition_openmp ||
      !same_result_openmp ||
      !BlockSort::are_valid_block_sizes(sequential_execution.blocks_before_sort) ||
      !BlockSort::is_sorted_non_decreasing(sequential_execution.result) ||
      !BlockSort::preserves_elements(data, sequential_execution.result) ||
      !BlockSort::are_valid_block_sizes(threads_execution.blocks_before_sort) ||
      !BlockSort::is_sorted_non_decreasing(threads_execution.result) ||
      !BlockSort::preserves_elements(data, threads_execution.result) ||
      !BlockSort::are_valid_block_sizes(openmp_execution.blocks_before_sort) ||
      !BlockSort::is_sorted_non_decreasing(openmp_execution.result) ||
      !BlockSort::preserves_elements(data, openmp_execution.result)) {
    return 1;
  }

  return 0;
}
