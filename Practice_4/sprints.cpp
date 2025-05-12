#include <vector>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <random>
#include <thread>
#include <iostream>



constexpr const int THREADS            = 4;
constexpr const int DATA_SIZE          = 10'000;
constexpr const int ARBITRARY_INIT_VAL = 51;
constexpr const int ITERATIONS         = 5;



void f_generated_data( std::vector <int>& v ) {
   std::random_device rd;
   std::mt19937 gen( rd() );
   std::uniform_int_distribution <> dis(0, DATA_SIZE);

   std::generate( v.begin(), v.end(), [&]() { return dis(gen); } );
}

void sprint_worker( const std::vector <int>& v, int target, std::atomic <double>& result ) {
   if ( v.empty() ) { result = 0.0; return; }

   auto pos = std::find( v.begin(), v.end(), target );
   auto end = ( (pos != v.end()) ? pos : v.end() );

   if (v.begin() == end) { result = 0.0; return; }

   result = static_cast <double> ( std::accumulate(v.begin(), end, 0) ) / std::distance( v.begin(), end );
}



int main(int argc, char* argv[]) {
   /* generate random data sets: */
   std::vector < std::vector <int> > data_sets;
   for (int i = 0; i < THREADS; ++i) {
      std::vector <int> v(DATA_SIZE);
      f_generated_data(v); v.shrink_to_fit();
      data_sets.emplace_back( std::move(v) );
   }

   std::vector < std::atomic <double> > results(THREADS);
   int target = ARBITRARY_INIT_VAL;

   for (int iteration = 0; iteration < ITERATIONS; ++iteration) {
      std::vector <std::thread> threads;

      for (int i = 0; i < THREADS; ++i)
         threads.emplace_back( sprint_worker, std::cref( data_sets[i] ), target, std::ref( results[i] ) );

      for (auto& t : threads) { t.join(); }

      double sum = 0.0;
      for (int i = 0; i < THREADS; ++i) { sum += results[i].load(); }

      target = static_cast <int> (sum / THREADS);

      std::cout << "Iteration " << iteration + 1 << ": new target = " << target << '\n';
   }

   return 0;
}