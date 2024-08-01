#ifndef UNIFORMRANDNUMGEN_HH
#define UNIFORMRANDNUMGEN_HH


#if __cplusplus < 201703L
#error "C++17 and above required due to use of variable templates \
std::is_integral_v and std::is_floating_point_v"
#endif

#include <random>
#include <type_traits>

// Note: virtual function table often creates performance overhead, see:
//   - https://stackoverflow.com/a/667680
template<typename T>
class UniformRandNumGen {
public:
    UniformRandNumGen() :rd{}, rng{rd()} {}
    virtual ~UniformRandNumGen() {}
    virtual T operator()() = 0;
private:
    std::random_device rd;     // only used once to initialise (seed) engine
protected:
    // The default_random_engine (minstd_rand0 or
    //   std::linear_congruential_engine<std::uint_fast32_t, 16807, 0,
    //   2147483647> in the GNU implementation) seems to not be
    //   seeded/produces the same values at runtime for every compilation;
    //   using Marsenne instead.
    std::mt19937 rng;          // random-number engine used (Mersenne-Twister)
};

template<typename IntT,
         typename = std::enable_if_t<std::is_integral_v<IntT>>>
class UniformRandIntGen : public UniformRandNumGen<IntT> {
public:
    UniformRandIntGen(IntT low, IntT high) :dist{low, high} {}
    IntT operator()() { return dist(this->rng); }
private:
    std::uniform_int_distribution<IntT> dist;
};

template<typename RealT,
         typename = std::enable_if_t<std::is_floating_point_v<RealT>>>
class UniformRandRealGen : public UniformRandNumGen<RealT> {
public:
    UniformRandRealGen(RealT low, RealT high) :dist{low, high} {}
    RealT operator()() { return dist(this->rng); }
private:
    std::uniform_real_distribution<RealT> dist;
};


#endif  // UNIFORMRANDNUMGEN_HH
