#include "Rand.hpp"

#include <random>

static thread_local std::default_random_engine generator(std::random_device{}());

double randNormal(double mean, double stdDev) {
    std::normal_distribution<double> normalDistribution(mean, stdDev);
    return normalDistribution(generator);
}

unsigned int randPositiveInt(unsigned int max) {
    std::uniform_int_distribution<unsigned int> distribution(1, max);
    return distribution(generator);
}

unsigned long randPositiveLong(unsigned long max) {
    std::uniform_int_distribution<unsigned long> distribution(1, max);
    return distribution(generator);
}
