#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <algorithm>
#include <execution>

constexpr std::size_t COUNT_EL = 1'000'000;

inline std::vector <int> generateData() {
  std::vector <int> v(COUNT_EL);
  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution <int> data_dist(0, 0xFFFF'FFFF);
  for (auto& x : v) { x = data_dist(rng); }

  return v;
}

static void BM_reduce_par(benchmark::State& state) {
   const auto data = generateData();
   for (auto _ : state) {
      auto result = std::reduce( std::execution::par, data.begin(), data.end() ); 
      benchmark::DoNotOptimize(result);
   }
} 

static void BM_reduce_seq(benchmark::State& state) {
   const auto data = generateData();
   for (auto _ : state) {
      auto result = std::reduce( std::execution::seq, data.begin(), data.end() );
      benchmark::DoNotOptimize(result);
   }
}

static void BM_sort_par(benchmark::State& state) {
  for (auto _ : state) {
    auto data = generateData();
    std::sort(std::execution::par, data.begin(), data.end());
  }
}

static void BM_sort_seq(benchmark::State& state) {
  for (auto _ : state) {
    auto data = generateData();
    std::sort( std::execution::seq, data.begin(), data.end() );
  }
}

BENCHMARK(BM_reduce_par);
BENCHMARK(BM_reduce_seq);
BENCHMARK(BM_sort_par);
BENCHMARK(BM_sort_seq);

BENCHMARK_MAIN();

/*
Выводы:
  std::execution::par работает быстрее в данных контекстах, потому что
  параллелизация делит задачу на части. Без параллелизации O(n) -> С
  параллелизацией O(n) делится. std::reduce() используя параллельное исполнение,
  почти в 2 раза быстрее последовательного исполнения. std::sort - более сложная
  операция, поэтому и итоговый результат лучше почти в 4 раза. std::sort с
  std::execution::par сортирует одновременно на разных потоках а далее мерджит их.
  Дополнительный вывод: РЕЗУЛЬТАТЫ ЗАВИСЯТ ОТ ЖЕЛЕЗА
  Общие результаты тестов:
  --------------------------------------------------------
  Benchmark              Time             CPU   Iterations
  --------------------------------------------------------
  BM_reduce_par      44652 ns        37667 ns        22400
  BM_reduce_seq      78470 ns        78474 ns         8960
  BM_sort_par     19118086 ns     18158784 ns           37
  BM_sort_seq     79222186 ns     78125000 ns            7
  --------------------------------------------------------
*/