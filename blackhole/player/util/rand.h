#ifndef _LESS_RAND
#define _LESS_RAND

#include <cstdint>
#include <random>

class Rand{
public:
    void seed();
    uint32_t operator()();
private:
    std::mt19937 generator;
};

extern Rand rand_gen;

#endif
