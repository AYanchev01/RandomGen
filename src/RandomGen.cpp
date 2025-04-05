#include "../include/RandomGen.h"
#include <numeric>
#include <cmath>
#include <string>

std::mt19937 RandomGen::gen(std::random_device{}());
std::mutex RandomGen::genMutex;

RandomGen::RandomGen(const std::vector<int>& randomNums, const std::vector<double>& probabilities)
    : randomNums(randomNums), 
      probabilities(probabilities),
      dist(0.0, 1.0)
{
    // Validate inputs
    if (randomNums.size() != probabilities.size()) {
        throw std::invalid_argument("Length of randomNums and probabilities must be equal");
    }
    
    if (probabilities.empty()) {
        throw std::invalid_argument("Probabilities cannot be empty");
    }
    
    // Check if probabilities sum to 1
    double sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);

    constexpr double EPSILON = 1e-10;
    if (std::abs(sum - 1.0) > EPSILON) {
        throw std::invalid_argument("Probabilities must sum to 1, got: " + std::to_string(sum));
    }
    
    // Check if any probability is negative
    if (std::any_of(probabilities.begin(), probabilities.end(), 
                   [](double p) { return p < 0; })) {
        throw std::invalid_argument("Probabilities must be non-negative");
    }
    
    // Precompute cumulative probabilities
    cumulativeProbabilities.resize(probabilities.size());
    std::partial_sum(probabilities.begin(), probabilities.end(), 
                     cumulativeProbabilities.begin());
}

int RandomGen::nextNum() noexcept {
    // Generate a random number between 0 and 1
    double randomValue;
    {
        std::lock_guard<std::mutex> lock(genMutex);
        randomValue = dist(gen);
    }
    
    // Find the index where randomValue would be inserted in cumulativeProbabilities
    auto it = std::lower_bound(cumulativeProbabilities.begin(), 
                              cumulativeProbabilities.end(), 
                              randomValue);
    
    // Get the index and return the corresponding random number
    size_t index = std::distance(cumulativeProbabilities.begin(), it);

    // Safety check for floating point precision issues
    if (index >= randomNums.size()) {
        return randomNums.back();
    }
    return randomNums[index];
}

void RandomGen::setSeed(unsigned int seed) {
    std::lock_guard<std::mutex> lock(genMutex);
    gen.seed(seed);
}