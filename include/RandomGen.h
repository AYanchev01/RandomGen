#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <mutex>

/**
 * @brief RandomGen class that returns random numbers based on specified probabilities
 */
class RandomGen {
private:
    std::vector<int> randomNums;
    std::vector<double> probabilities;
    std::vector<double> cumulativeProbabilities;
    
    static std::mt19937 gen;
    static std::mutex genMutex;
    std::uniform_real_distribution<double> dist;

public:
    /**
     * @brief Construct a new RandomGen object
     * 
     * @param randomNums Values that may be returned by nextNum()
     * @param probabilities Probability of occurrence for each value
     * @throw std::invalid_argument if inputs are invalid
     */
    RandomGen(const std::vector<int>& randomNums, const std::vector<double>& probabilities);
    
    /**
     * @brief Returns one of the randomNums based on their probabilities
     * 
     * When called multiple times over a long period, it returns numbers
     * roughly with the initialized probabilities.
     * 
     * This method is thread-safe and can be called concurrently from multiple threads.
     * 
     * @return int A randomly selected number
     */
    int nextNum() noexcept;
    
    /**
     * @brief Sets a custom seed for the random number generator
     * 
     * This method affects all instances of RandomGen across all threads.
     * 
     * @param seed The seed value to use
     */
    static void setSeed(unsigned int seed);
};