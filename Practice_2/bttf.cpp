#include <future>
#include <vector>
#include <numeric>

std::future <int> avg_future( const std::vector <int>& data) {
	return std::async(std::launch::async, [data]() {
		if ( data.empty() )	{ return 0; }
		int sum = std::accumulate(data.begin(), data.end(), 0);
		return sum / static_cast <int> ( data.size() );
	});
}
