
#ifndef PROXYFMU_FIXED_RANGE_RANDOM_GENERATOR_HPP
#define PROXYFMU_FIXED_RANGE_RANDOM_GENERATOR_HPP

#include <random>

namespace proxyfmu
{

class fixed_range_random_generator
{

private:
    std::mt19937 mt_;
    std::uniform_int_distribution<int> dist_;

public:
    fixed_range_random_generator(int min, int max)
        : mt_(std::random_device()())
        , dist_(min, max)
    {
    }

    int next()
    {
        return dist_(mt_);
    }
};

} // namespace proxyfmu

#endif // PROXYFMU_FIXED_RANGE_RANDOM_GENERATOR_HPP
