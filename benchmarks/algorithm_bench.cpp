/*
Run on (8 X 4008 MHz CPU s)
10/13/17 11:21:46
------------------------------------------------------------------
Benchmark                           Time           CPU Iterations
------------------------------------------------------------------
StableSort/5/64                   808 ns        774 ns     746667
StableSort/5/512                 7348 ns       7499 ns      89600
StableSort/5/4096               68220 ns      64174 ns      11200
StableSort/5/32768             683621 ns     593750 ns       1000
StableSort/5/262144           6118716 ns    6250000 ns         90
StableSort/20/64                  908 ns        900 ns     746667
StableSort/20/512                9294 ns      10498 ns      64000
StableSort/20/4096              84527 ns      87847 ns       9249
StableSort/20/32768            812946 ns     899431 ns        747
StableSort/20/262144          7114586 ns    7393973 ns        112
StableSort/100/64                 932 ns        879 ns    1120000
StableSort/100/512              11306 ns      10930 ns      74335
StableSort/100/4096            108063 ns      97656 ns       6400
StableSort/100/32768           984662 ns    1025391 ns        640
StableSort/100/262144         8562026 ns    8333333 ns         75
StableSegregate/5/64              892 ns        753 ns     746667
StableSegregate/5/512            4614 ns       4290 ns     149333
StableSegregate/5/4096          33644 ns      30134 ns      24889
StableSegregate/5/32768        266188 ns     262277 ns       2800
StableSegregate/5/262144      2265832 ns    2083333 ns        345
StableSegregate/20/64            1635 ns       1674 ns     373333
StableSegregate/20/512          10189 ns      10254 ns      74667
StableSegregate/20/4096         59508 ns      52828 ns      11535
StableSegregate/20/32768       455795 ns     435287 ns       1723
StableSegregate/20/262144     4048701 ns    3765060 ns        166
StableSegregate/100/64           1560 ns       1674 ns     746667
StableSegregate/100/512         30493 ns      32959 ns      21333
StableSegregate/100/4096       166070 ns     163218 ns       4978
StableSegregate/100/32768     1199679 ns    1129518 ns        498
StableSegregate/100/262144   10730091 ns   10625000 ns         75
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
 * \param f, l	            the range of elements to segregate
 * \param partition_points  beginning of the destination that will store all partition points
 * \param Pred              binary predicate which returns true for matching elements
 *
 * \returns the new end of the partition points range
 *
 * \pre [f,l) is a valid range
 * \note *f will not move
  * \note an earlier name was stable_adjacent_partition
 */
template< typename BidirIt, typename OutIt, typename BinaryPred >
OutIt stable_segregate( BidirIt f, BidirIt l, OutIt partition_points, BinaryPred p )
{
    while ( f != l ) {
        auto & q = *f;
        auto is_match = [&]( auto && x ) -> bool { return p(q,x); };
        f = std::stable_partition( ++f, l, is_match );
        *partition_points = f;
        ++partition_points;
    }

    return partition_points;
}

template< typename BidirIt, typename OutIt >
OutIt stable_segregate( BidirIt f, BidirIt l, OutIt partition_points )
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
