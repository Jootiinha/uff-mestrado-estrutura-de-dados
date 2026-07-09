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
inline bool is_sorted_non_decreasing(const std::vector<int> &data);
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
  std::ostringstream out;
  out << "pares=" << algorithm_name(combination.even_block)
      << ", impares=" << algorithm_name(combination.odd_block)
      << ", final=" << algorithm_name(combination.final_stage);
  return out.str();
}

inline bool is_valid_combination(const BlockSortCombination &combination) {
  const bool even_ok =
      combination.even_block == BlockSortAlgorithm::SelectionSort ||
      combination.even_block == BlockSortAlgorithm::MergeSort ||
      combination.even_block == BlockSortAlgorithm::InsertionSort;

  const bool odd_ok =
      combination.odd_block == BlockSortAlgorithm::SelectionSort ||
      combination.odd_block == BlockSortAlgorithm::MergeSort ||
      combination.odd_block == BlockSortAlgorithm::BubbleSort;

  const bool final_ok =
      combination.final_stage == BlockSortAlgorithm::InsertionSort ||
      combination.final_stage == BlockSortAlgorithm::MergeSort ||
      combination.final_stage == BlockSortAlgorithm::QuickSort;

  if (!even_ok || !odd_ok || !final_ok) {
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

inline bool can_partition_into_blocks_4_or_5(int n) {
  if (n < 0) {
    return false;
  }

  std::vector<bool> dp(static_cast<size_t>(n) + 1, false);
  dp[0] = true;

  for (int i = 0; i <= n; ++i) {
    if (!dp[static_cast<size_t>(i)]) {
      continue;
    }
    if (i + 4 <= n) {
      dp[static_cast<size_t>(i + 4)] = true;
    }
    if (i + 5 <= n) {
      dp[static_cast<size_t>(i + 5)] = true;
    }
  }

  return dp[static_cast<size_t>(n)];
}

inline std::vector<std::vector<int>>
partition_blocks(const std::vector<int> &data, std::uint32_t seed) {
  if (!can_partition_into_blocks_4_or_5(static_cast<int>(data.size()))) {
    throw std::invalid_argument(
        "Input size cannot be partitioned into blocks of 4 or 5.");
  }

  std::mt19937 rng(seed);
  std::vector<std::vector<int>> blocks;
  size_t index = 0;

  while (index < data.size()) {
    const int remaining = static_cast<int>(data.size() - index);
    std::vector<int> valid_sizes;

    for (int candidate : {4, 5}) {
      if (remaining >= candidate &&
          can_partition_into_blocks_4_or_5(remaining - candidate)) {
        valid_sizes.push_back(candidate);
      }
    }

    if (valid_sizes.empty()) {
      throw std::logic_error("Failed to build a valid partition.");
    }

    std::uniform_int_distribution<size_t> dist(0, valid_sizes.size() - 1);
    const int block_size = valid_sizes[dist(rng)];

    blocks.emplace_back(data.begin() + static_cast<long>(index),
                        data.begin() + static_cast<long>(index + block_size));
    index += static_cast<size_t>(block_size);
  }

  return blocks;
}

inline void apply_algorithm(std::vector<int> &data, BlockSortAlgorithm algo) {
  switch (algo) {
  case BlockSortAlgorithm::InsertionSort:
    InsertionSort().sort(data);
    return;
  case BlockSortAlgorithm::SelectionSort:
    SelectionSort().sort(data);
    return;
  case BlockSortAlgorithm::BubbleSort:
    BubbleSort().sort(data);
    return;
  case BlockSortAlgorithm::MergeSort:
    MergeSort().sort(data);
    return;
  case BlockSortAlgorithm::QuickSort:
    QuickSort().sort(data);
    return;
  }
}

inline std::vector<int> merge_two_sorted_vectors(const std::vector<int> &left,
                                                 const std::vector<int> &right) {
  std::vector<int> merged;
  merged.reserve(left.size() + right.size());

  size_t i = 0;
  size_t j = 0;

  while (i < left.size() && j < right.size()) {
    if (left[i] <= right[j]) {
      merged.push_back(left[i++]);
    } else {
      merged.push_back(right[j++]);
    }
  }

  while (i < left.size()) {
    merged.push_back(left[i++]);
  }
  while (j < right.size()) {
    merged.push_back(right[j++]);
  }

  return merged;
}

inline std::vector<int>
merge_blocks(std::vector<std::vector<int>> blocks,
             BlockSortAlgorithm final_stage_algorithm) {
  if (blocks.empty()) {
    return {};
  }

  if (final_stage_algorithm == BlockSortAlgorithm::MergeSort) {
    while (blocks.size() > 1) {
      std::vector<std::vector<int>> next_level;
      next_level.reserve((blocks.size() + 1) / 2);

      for (size_t i = 0; i < blocks.size(); i += 2) {
        if (i + 1 == blocks.size()) {
          next_level.push_back(blocks[i]);
        } else {
          next_level.push_back(
              merge_two_sorted_vectors(blocks[i], blocks[i + 1]));
        }
      }

      blocks = std::move(next_level);
    }

    return blocks.front();
  }

  std::vector<int> flattened;
  for (const auto &block : blocks) {
    flattened.insert(flattened.end(), block.begin(), block.end());
  }

  apply_algorithm(flattened, final_stage_algorithm);
  return flattened;
}

inline BlockSortExecution
run_sequential(const std::vector<int> &data,
               const BlockSortCombination &combination = {},
               std::uint32_t seed = std::random_device{}()) {
  BlockSortExecution execution;
  execution.blocks_before_sort = partition_blocks(data, seed);
  execution.blocks_after_sort = execution.blocks_before_sort;

  for (auto &block : execution.blocks_after_sort) {
    const BlockSortAlgorithm block_algorithm =
        (block.size() % 2 == 0) ? combination.even_block : combination.odd_block;
    apply_algorithm(block, block_algorithm);
  }

  execution.result =
      merge_blocks(execution.blocks_after_sort, combination.final_stage);
  return execution;
}

inline BlockSortExecution
run_threads(const std::vector<int> &data,
            const BlockSortCombination &combination = {},
            std::uint32_t seed = std::random_device{}()) {
  BlockSortExecution execution;
  execution.blocks_before_sort = partition_blocks(data, seed);
  execution.blocks_after_sort = execution.blocks_before_sort;

  std::vector<std::thread> workers;
  workers.reserve(execution.blocks_after_sort.size());

  for (auto &block : execution.blocks_after_sort) {
    workers.emplace_back([&block, &combination]() {
      const BlockSortAlgorithm block_algorithm =
          (block.size() % 2 == 0) ? combination.even_block
                                  : combination.odd_block;
      apply_algorithm(block, block_algorithm);
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
run_openmp(const std::vector<int> &data,
           const BlockSortCombination &combination = {},
           std::uint32_t seed = std::random_device{}()) {
  BlockSortExecution execution;
  execution.blocks_before_sort = partition_blocks(data, seed);
  execution.blocks_after_sort = execution.blocks_before_sort;

#pragma omp parallel for
  for (int i = 0; i < static_cast<int>(execution.blocks_after_sort.size()); ++i) {
    auto &block = execution.blocks_after_sort[static_cast<size_t>(i)];
    const BlockSortAlgorithm block_algorithm =
        (block.size() % 2 == 0) ? combination.even_block
                                : combination.odd_block;
    apply_algorithm(block, block_algorithm);
  }

  execution.result =
      merge_blocks(execution.blocks_after_sort, combination.final_stage);
  return execution;
}

inline BlockSortExecution
run_mode(BlockSortMode mode, const std::vector<int> &data,
         const BlockSortCombination &combination = {},
         std::uint32_t seed = std::random_device{}()) {
  switch (mode) {
  case BlockSortMode::Sequential:
    return run_sequential(data, combination, seed);
  case BlockSortMode::Threads:
    return run_threads(data, combination, seed);
  case BlockSortMode::OpenMP:
    return run_openmp(data, combination, seed);
  }

  throw std::logic_error("Unknown block sort mode.");
}

inline std::vector<std::vector<int>>
make_benchmark_inputs(int size, int sample_count, int max_value,
                      std::uint32_t seed) {
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> dist(0, max_value);

  std::vector<std::vector<int>> inputs(static_cast<size_t>(sample_count),
                                       std::vector<int>(static_cast<size_t>(size)));

  for (auto &input : inputs) {
    for (int &value : input) {
      value = dist(rng);
    }
  }

  return inputs;
}

inline double benchmark_combination(
    const BlockSortCombination &combination,
    const std::vector<std::vector<int>> &inputs, std::uint32_t partition_seed) {
  using Clock = std::chrono::high_resolution_clock;

  double total_seconds = 0.0;
  std::uint32_t current_seed = partition_seed;

  for (const auto &input : inputs) {
    const auto start = Clock::now();
    const BlockSortExecution execution =
        run_sequential(input, combination, current_seed++);
    const auto end = Clock::now();

    if (!are_valid_block_sizes(execution.blocks_before_sort) ||
        !is_sorted_non_decreasing(execution.result) ||
        !preserves_elements(input, execution.result)) {
      throw std::logic_error("Invalid result during combination benchmark.");
    }

    total_seconds +=
        std::chrono::duration<double>(end - start).count();
  }

  if (inputs.empty()) {
    return 0.0;
  }

  return total_seconds / static_cast<double>(inputs.size());
}

inline std::vector<BlockSortBenchmarkSummary>
rank_combinations(const std::vector<std::vector<int>> &inputs,
                  std::uint32_t partition_seed) {
  std::vector<BlockSortBenchmarkSummary> ranking;

  for (const auto &combination : valid_combinations()) {
    ranking.push_back({combination,
                       benchmark_combination(combination, inputs,
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
select_best_combination(const std::vector<std::vector<int>> &inputs,
                        std::uint32_t partition_seed) {
  const auto ranking = rank_combinations(inputs, partition_seed);
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

  std::vector<int> left = original;
  std::vector<int> right = result;
  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());
  return left == right;
}

} // namespace BlockSort

#endif
