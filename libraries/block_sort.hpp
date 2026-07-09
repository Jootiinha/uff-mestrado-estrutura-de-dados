#ifndef BLOCK_SORT_HPP
#define BLOCK_SORT_HPP

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <random>
#include <sstream>
#include <string>
#include <stdexcept>
#include <thread>
#include <vector>

#include "bubble_sort.h"
#include "insertion_sort.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "selection_sort.h"

enum class BlockSortAlgorithm {
  InsertionSort,
  SelectionSort,
  BubbleSort,
  MergeSort,
  QuickSort,
};

enum class BlockSortMode {
  Sequential,
  Threads,
  OpenMP,
};

struct BlockSortCombination {
  BlockSortAlgorithm even_block = BlockSortAlgorithm::InsertionSort;
  BlockSortAlgorithm odd_block = BlockSortAlgorithm::SelectionSort;
  BlockSortAlgorithm final_stage = BlockSortAlgorithm::MergeSort;
};

struct BlockSortBenchmarkSummary {
  BlockSortCombination combination;
  double average_seconds = 0.0;
};

struct BlockSortExecution {
  std::vector<std::vector<int>> blocks_before_sort;
  std::vector<std::vector<int>> blocks_after_sort;
  std::vector<int> result;
};

namespace BlockSort {

inline bool are_valid_block_sizes(const std::vector<std::vector<int>> &blocks);
inline bool is_sorted_non_decreasing(const std::vector<int> &values);
inline bool preserves_elements(const std::vector<int> &original,
                               const std::vector<int> &result);

inline const char *algorithm_name(BlockSortAlgorithm algorithm) {
  switch (algorithm) {
  case BlockSortAlgorithm::InsertionSort:
    return "InsertionSort";
  case BlockSortAlgorithm::SelectionSort:
    return "SelectionSort";
  case BlockSortAlgorithm::BubbleSort:
    return "BubbleSort";
  case BlockSortAlgorithm::MergeSort:
    return "MergeSort";
  case BlockSortAlgorithm::QuickSort:
    return "QuickSort";
  }

  return "Unknown";
}

inline const char *mode_name(BlockSortMode mode) {
  switch (mode) {
  case BlockSortMode::Sequential:
    return "sequential";
  case BlockSortMode::Threads:
    return "threads";
  case BlockSortMode::OpenMP:
    return "openmp";
  }

  return "unknown";
}

inline std::string combination_name(const BlockSortCombination &combination) {
  std::ostringstream description;
  description << "pares=" << algorithm_name(combination.even_block)
              << ", impares=" << algorithm_name(combination.odd_block)
              << ", final=" << algorithm_name(combination.final_stage);
  return description.str();
}

inline bool is_valid_combination(const BlockSortCombination &combination) {
  const bool even_algorithm_is_allowed =
      combination.even_block == BlockSortAlgorithm::SelectionSort ||
      combination.even_block == BlockSortAlgorithm::MergeSort ||
      combination.even_block == BlockSortAlgorithm::InsertionSort;

  const bool odd_algorithm_is_allowed =
      combination.odd_block == BlockSortAlgorithm::SelectionSort ||
      combination.odd_block == BlockSortAlgorithm::MergeSort ||
      combination.odd_block == BlockSortAlgorithm::BubbleSort;

  const bool final_algorithm_is_allowed =
      combination.final_stage == BlockSortAlgorithm::InsertionSort ||
      combination.final_stage == BlockSortAlgorithm::MergeSort ||
      combination.final_stage == BlockSortAlgorithm::QuickSort;

  if (!even_algorithm_is_allowed || !odd_algorithm_is_allowed ||
      !final_algorithm_is_allowed) {
    return false;
  }

  return combination.even_block != combination.odd_block &&
         combination.even_block != combination.final_stage &&
         combination.odd_block != combination.final_stage;
}

inline std::vector<BlockSortCombination> valid_combinations() {
  std::vector<BlockSortCombination> combinations;

  const BlockSortAlgorithm even_candidates[] = {
      BlockSortAlgorithm::SelectionSort, BlockSortAlgorithm::MergeSort,
      BlockSortAlgorithm::InsertionSort};
  const BlockSortAlgorithm odd_candidates[] = {
      BlockSortAlgorithm::SelectionSort, BlockSortAlgorithm::MergeSort,
      BlockSortAlgorithm::BubbleSort};
  const BlockSortAlgorithm final_candidates[] = {
      BlockSortAlgorithm::InsertionSort, BlockSortAlgorithm::MergeSort,
      BlockSortAlgorithm::QuickSort};

  for (const auto even_algorithm : even_candidates) {
    for (const auto odd_algorithm : odd_candidates) {
      for (const auto final_algorithm : final_candidates) {
        BlockSortCombination combination{even_algorithm, odd_algorithm,
                                         final_algorithm};
        if (is_valid_combination(combination)) {
          combinations.push_back(combination);
        }
      }
    }
  }

  return combinations;
}

inline bool can_partition_into_blocks_4_or_5(int total_size) {
  if (total_size < 0) {
    return false;
  }

  std::vector<bool> reachable_sizes(static_cast<size_t>(total_size) + 1, false);
  reachable_sizes[0] = true;

  for (int current_size = 0; current_size <= total_size; ++current_size) {
    if (!reachable_sizes[static_cast<size_t>(current_size)]) {
      continue;
    }
    if (current_size + 4 <= total_size) {
      reachable_sizes[static_cast<size_t>(current_size + 4)] = true;
    }
    if (current_size + 5 <= total_size) {
      reachable_sizes[static_cast<size_t>(current_size + 5)] = true;
    }
  }

  return reachable_sizes[static_cast<size_t>(total_size)];
}

inline std::vector<std::vector<int>>
partition_blocks(const std::vector<int> &values, std::uint32_t seed) {
  if (!can_partition_into_blocks_4_or_5(static_cast<int>(values.size()))) {
    throw std::invalid_argument(
        "Input size cannot be partitioned into blocks of 4 or 5.");
  }

  std::mt19937 random_engine(seed);
  std::vector<std::vector<int>> blocks;
  size_t next_value_index = 0;

  while (next_value_index < values.size()) {
    const int remaining_values =
        static_cast<int>(values.size() - next_value_index);
    std::vector<int> candidate_block_sizes;

    for (int candidate : {4, 5}) {
      if (remaining_values >= candidate &&
          can_partition_into_blocks_4_or_5(remaining_values - candidate)) {
        candidate_block_sizes.push_back(candidate);
      }
    }

    if (candidate_block_sizes.empty()) {
      throw std::logic_error("Failed to build a valid partition.");
    }

    std::uniform_int_distribution<size_t> block_size_distribution(
        0, candidate_block_sizes.size() - 1);
    const int block_size =
        candidate_block_sizes[block_size_distribution(random_engine)];

    blocks.emplace_back(
        values.begin() + static_cast<long>(next_value_index),
        values.begin() + static_cast<long>(next_value_index + block_size));
    next_value_index += static_cast<size_t>(block_size);
  }

  return blocks;
}

inline void apply_algorithm(std::vector<int> &values,
                            BlockSortAlgorithm algorithm) {
  switch (algorithm) {
  case BlockSortAlgorithm::InsertionSort:
    InsertionSort().sort(values);
    return;
  case BlockSortAlgorithm::SelectionSort:
    SelectionSort().sort(values);
    return;
  case BlockSortAlgorithm::BubbleSort:
    BubbleSort().sort(values);
    return;
  case BlockSortAlgorithm::MergeSort:
    MergeSort().sort(values);
    return;
  case BlockSortAlgorithm::QuickSort:
    QuickSort().sort(values);
    return;
  }
}

inline std::vector<int>
merge_two_sorted_vectors(const std::vector<int> &left_values,
                         const std::vector<int> &right_values) {
  std::vector<int> merged;
  merged.reserve(left_values.size() + right_values.size());

  size_t left_index = 0;
  size_t right_index = 0;

  while (left_index < left_values.size() &&
         right_index < right_values.size()) {
    if (left_values[left_index] <= right_values[right_index]) {
      merged.push_back(left_values[left_index++]);
    } else {
      merged.push_back(right_values[right_index++]);
    }
  }

  while (left_index < left_values.size()) {
    merged.push_back(left_values[left_index++]);
  }
  while (right_index < right_values.size()) {
    merged.push_back(right_values[right_index++]);
  }

  return merged;
}

inline std::vector<int>
merge_blocks(std::vector<std::vector<int>> sorted_blocks,
             BlockSortAlgorithm final_stage_algorithm) {
  if (sorted_blocks.empty()) {
    return {};
  }

  if (final_stage_algorithm == BlockSortAlgorithm::MergeSort) {
    while (sorted_blocks.size() > 1) {
      std::vector<std::vector<int>> merged_block_level;
      merged_block_level.reserve((sorted_blocks.size() + 1) / 2);

      for (size_t block_index = 0; block_index < sorted_blocks.size();
           block_index += 2) {
        if (block_index + 1 == sorted_blocks.size()) {
          merged_block_level.push_back(sorted_blocks[block_index]);
        } else {
          merged_block_level.push_back(merge_two_sorted_vectors(
              sorted_blocks[block_index], sorted_blocks[block_index + 1]));
        }
      }

      sorted_blocks = std::move(merged_block_level);
    }

    return sorted_blocks.front();
  }

  std::vector<int> flattened_values;
  for (const auto &block : sorted_blocks) {
    flattened_values.insert(flattened_values.end(), block.begin(), block.end());
  }

  apply_algorithm(flattened_values, final_stage_algorithm);
  return flattened_values;
}

inline BlockSortExecution
run_sequential(const std::vector<int> &values,
               const BlockSortCombination &combination = {},
               std::uint32_t seed = std::random_device{}()) {
  BlockSortExecution execution;
  execution.blocks_before_sort = partition_blocks(values, seed);
  execution.blocks_after_sort = execution.blocks_before_sort;

  for (auto &block : execution.blocks_after_sort) {
    const BlockSortAlgorithm selected_block_algorithm =
        (block.size() % 2 == 0) ? combination.even_block : combination.odd_block;
    apply_algorithm(block, selected_block_algorithm);
  }

  execution.result =
      merge_blocks(execution.blocks_after_sort, combination.final_stage);
  return execution;
}

inline BlockSortExecution
run_threads(const std::vector<int> &values,
            const BlockSortCombination &combination = {},
            std::uint32_t seed = std::random_device{}()) {
  BlockSortExecution execution;
  execution.blocks_before_sort = partition_blocks(values, seed);
  execution.blocks_after_sort = execution.blocks_before_sort;

  std::vector<std::thread> workers;
  workers.reserve(execution.blocks_after_sort.size());

  for (auto &block : execution.blocks_after_sort) {
    workers.emplace_back([&block, &combination]() {
      const BlockSortAlgorithm selected_block_algorithm =
          (block.size() % 2 == 0) ? combination.even_block
                                  : combination.odd_block;
      apply_algorithm(block, selected_block_algorithm);
    });
  }

  for (auto &worker : workers) {
    worker.join();
  }

  execution.result =
      merge_blocks(execution.blocks_after_sort, combination.final_stage);
  return execution;
}

inline BlockSortExecution
run_openmp(const std::vector<int> &values,
           const BlockSortCombination &combination = {},
           std::uint32_t seed = std::random_device{}()) {
  BlockSortExecution execution;
  execution.blocks_before_sort = partition_blocks(values, seed);
  execution.blocks_after_sort = execution.blocks_before_sort;

#pragma omp parallel for
  for (int block_index = 0;
       block_index < static_cast<int>(execution.blocks_after_sort.size());
       ++block_index) {
    auto &block =
        execution.blocks_after_sort[static_cast<size_t>(block_index)];
    const BlockSortAlgorithm selected_block_algorithm =
        (block.size() % 2 == 0) ? combination.even_block
                                : combination.odd_block;
    apply_algorithm(block, selected_block_algorithm);
  }

  execution.result =
      merge_blocks(execution.blocks_after_sort, combination.final_stage);
  return execution;
}

inline BlockSortExecution
run_mode(BlockSortMode mode, const std::vector<int> &values,
         const BlockSortCombination &combination = {},
         std::uint32_t seed = std::random_device{}()) {
  switch (mode) {
  case BlockSortMode::Sequential:
    return run_sequential(values, combination, seed);
  case BlockSortMode::Threads:
    return run_threads(values, combination, seed);
  case BlockSortMode::OpenMP:
    return run_openmp(values, combination, seed);
  }

  throw std::logic_error("Unknown block sort mode.");
}

inline std::vector<std::vector<int>>
make_benchmark_inputs(int size, int sample_count, int max_value,
                      std::uint32_t seed) {
  std::mt19937 random_engine(seed);
  std::uniform_int_distribution<int> value_distribution(0, max_value);

  std::vector<std::vector<int>> samples(
      static_cast<size_t>(sample_count),
      std::vector<int>(static_cast<size_t>(size)));

  for (auto &sample : samples) {
    for (int &value : sample) {
      value = value_distribution(random_engine);
    }
  }

  return samples;
}

inline double benchmark_combination(
    const BlockSortCombination &combination,
    const std::vector<std::vector<int>> &samples, std::uint32_t partition_seed) {
  using Clock = std::chrono::high_resolution_clock;

  double total_seconds = 0.0;
  std::uint32_t current_partition_seed = partition_seed;

  for (const auto &sample : samples) {
    const auto start_time = Clock::now();
    const BlockSortExecution execution =
        run_sequential(sample, combination, current_partition_seed++);
    const auto end_time = Clock::now();

    if (!are_valid_block_sizes(execution.blocks_before_sort) ||
        !is_sorted_non_decreasing(execution.result) ||
        !preserves_elements(sample, execution.result)) {
      throw std::logic_error("Invalid result during combination benchmark.");
    }

    total_seconds += std::chrono::duration<double>(end_time - start_time).count();
  }

  if (samples.empty()) {
    return 0.0;
  }

  return total_seconds / static_cast<double>(samples.size());
}

inline std::vector<BlockSortBenchmarkSummary>
rank_combinations(const std::vector<std::vector<int>> &samples,
                  std::uint32_t partition_seed) {
  std::vector<BlockSortBenchmarkSummary> ranking;

  for (const auto &combination : valid_combinations()) {
    ranking.push_back({combination,
                       benchmark_combination(combination, samples,
                                             partition_seed)});
  }

  std::sort(ranking.begin(), ranking.end(),
            [](const BlockSortBenchmarkSummary &left,
               const BlockSortBenchmarkSummary &right) {
              return left.average_seconds < right.average_seconds;
            });

  return ranking;
}

inline BlockSortBenchmarkSummary
select_best_combination(const std::vector<std::vector<int>> &samples,
                        std::uint32_t partition_seed) {
  const auto ranking = rank_combinations(samples, partition_seed);
  if (ranking.empty()) {
    throw std::logic_error("No valid block sort combinations found.");
  }
  return ranking.front();
}

inline bool are_valid_block_sizes(
    const std::vector<std::vector<int>> &blocks) {
  for (const auto &block : blocks) {
    if (block.size() != 4 && block.size() != 5) {
      return false;
    }
  }
  return true;
}

inline bool is_sorted_non_decreasing(const std::vector<int> &data) {
  return std::is_sorted(data.begin(), data.end());
}

inline bool preserves_elements(const std::vector<int> &original,
                               const std::vector<int> &result) {
  if (original.size() != result.size()) {
    return false;
  }

  std::vector<int> sorted_original_values = original;
  std::vector<int> sorted_result_values = result;
  std::sort(sorted_original_values.begin(), sorted_original_values.end());
  std::sort(sorted_result_values.begin(), sorted_result_values.end());
  return sorted_original_values == sorted_result_values;
}

} // namespace BlockSort

#endif
