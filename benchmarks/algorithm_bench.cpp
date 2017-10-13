/*
Run on( 8 X 4008 MHz CPU s )
10 / 13 / 17 02:37 : 48
------------------------------------------------------------------
Benchmark                           Time           CPU Iterations
------------------------------------------------------------------
StableSort / 5 / 64                   805 ns        858 ns     746667
StableSort / 5 / 512                 7372 ns       7150 ns      89600
StableSort / 5 / 4096               68138 ns      71933 ns      13033
StableSort / 5 / 32768             694004 ns     703125 ns       1000
StableSort / 5 / 262144           6071027 ns    5937500 ns        100
StableSort / 20 / 64                  907 ns        830 ns     640000
StableSort / 20 / 512                9292 ns      10254 ns      64000
StableSort / 20 / 4096              84552 ns      83705 ns       8960
StableSort / 20 / 32768            818910 ns     857597 ns        747
StableSort / 20 / 262144          7122335 ns    7254464 ns        112
StableSort / 100 / 64                 934 ns        903 ns     640000
StableSort / 100 / 512              11332 ns      11719 ns      56000
StableSort / 100 / 4096            108316 ns      89286 ns       5600
StableSort / 100 / 32768           994719 ns     920759 ns        560
StableSort / 100 / 262144         8570463 ns    7986111 ns         90
StableSegregate / 5 / 64              901 ns        900 ns     746667
StableSegregate / 5 / 512            4617 ns       3868 ns     145438
StableSegregate / 5 / 4096          33847 ns      36272 ns      17231
StableSegregate / 5 / 32768        263493 ns     268555 ns       3200
StableSegregate / 5 / 262144      2235997 ns    2064732 ns        280
StableSegregate / 20 / 64            1633 ns       1569 ns     448000
StableSegregate / 20 / 512          10111 ns      10045 ns      74667
StableSegregate / 20 / 4096         62014 ns      73242 ns       8960
StableSegregate / 20 / 32768       454030 ns     429086 ns       1493
StableSegregate / 20 / 262144     4018690 ns    4269622 ns        172
StableSegregate / 100 / 64           1579 ns       1496 ns     407273
StableSegregate / 100 / 512         30081 ns      28878 ns      24889
StableSegregate / 100 / 4096       166012 ns     172635 ns       4978
StableSegregate / 100 / 32768     1203770 ns    1296854 ns        747
StableSegregate / 100 / 262144   10426475 ns   11041667 ns         75
*/

#include <random>
#include <vector>
#include <iterator>
#include <algorithm>
#include <benchmark/benchmark.h>

class null_output_iterator
    : public std::iterator<std::output_iterator_tag,void,void,void,void>
{
public:
    template<typename T>
    null_output_iterator &operator=(const T &) { return *this; }
    null_output_iterator &operator*() { return *this; }
    null_output_iterator &operator++() { return *this; }
    null_output_iterator operator++(int) { return *this; }
};

/*! \brief  stable partition a range into subsets of matching elements
 *
 * \details
 * reorders the elements in the range [f,l) such that:
 * - all matching elements are adjacent
 * - the relative order of matching elements is preserved
 * - the relative order of the first elements of each partition are preserved
 *
 * elements match if they compare equal using operator== or pred
 *
 * \param f, l	        	the range of elements to segregate
 * \param partition_points  an output iterator that each partition point is written to
 * \param Pred              binary predicate which returns true for matching elements
 *
 * \returns the number of partition points
 *
 * \pre [f,l) is a valid range
 * \note *f will not move
  * \note an earlier name was stable_adjacent_partition
 */
template< typename I, typename O, typename Pred >
size_t stable_segregate( I f, I l, O partition_points, Pred p )
{
    size_t num_partition_points = 0;
    while ( f != l ) {
        auto & q = *f;
        auto is_match = [&]( auto && x ) -> bool { return p(q,x); };
        f = std::stable_partition( ++f, l, is_match );
        *partition_points = f;
        ++partition_points;
        ++num_partition_points;
    }

    return num_partition_points;
}

template< typename I, typename O >
size_t stable_segregate( I f, I l, O partition_points )
{
    return stable_segregate( f, l, partition_points, std::equal_to<>() );
}

static void StableSort( benchmark::State& state )
{
    std::mt19937 rng( std::mt19937::default_seed );
    std::vector<int> data;
    std::uniform_int_distribution<int> dist( 0, state.range_x() );
    while ( state.KeepRunning() ) {
        state.PauseTiming();
        const auto n = state.range_y();
        data.resize( n );
        std::generate_n( data.data(), n, [&](){ return dist( rng ); } );
        state.ResumeTiming();

        std::stable_sort (begin( data ), end( data ));
    }
}

static void StableSegregate( benchmark::State& state )
{
    std::mt19937 rng( std::mt19937::default_seed );
    std::vector<int> data;
    std::uniform_int_distribution<int> dist( 0, state.range_x() );
    while ( state.KeepRunning() ) {
        state.PauseTiming();
        const auto n = state.range_y();
        data.resize( n );
        std::generate_n( data.data(), n, [&](){ return dist( rng ); } );
        state.ResumeTiming();

        stable_segregate( begin(data), end(data), null_output_iterator() );
    }
}

constexpr int size_min          = 64;
constexpr int size_max          = 1'048'576;
constexpr int size_multiplier   = 8;
static void CustomArguments( benchmark::internal::Benchmark* b )
{
    for ( int i : { 5, 20, 100 } ) {
        for ( int j = size_min; j <= size_max; j *= size_multiplier ) {
            b->Args( {i, j} );
        }
    }
}


BENCHMARK( StableSort )->Apply(CustomArguments);
BENCHMARK( StableSegregate )->Apply( CustomArguments );

BENCHMARK_MAIN()
