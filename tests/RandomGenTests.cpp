#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "RandomGen.h"
#include <unordered_map>
#include <memory>
#include <thread>
#include <future>

// Test initialization with valid parameters
TEST(RandomGen, InitializationValid) {
    std::vector<int> randomNums = {1, 2, 3};
    std::vector<double> probabilities = {0.2, 0.5, 0.3};
    
    EXPECT_NO_THROW({
        RandomGen randomGen(randomNums, probabilities);
    });
}

// Test initialization with probabilities that don't sum to 1
TEST(RandomGen, InitializationInvalidSum) {
    std::vector<int> randomNums = {1, 2, 3};
    std::vector<double> probabilities = {0.2, 0.2, 0.2}; // Sum is 0.6
    
    EXPECT_THROW({
        RandomGen randomGen(randomNums, probabilities);
    }, std::invalid_argument);
}

// Test initialization with negative probabilities
TEST(RandomGen, InitializationNegativeProbabilities) {
    std::vector<int> randomNums = {1, 2, 3};
    std::vector<double> probabilities = {0.2, -0.2, 1.0};
    
    EXPECT_THROW({
        RandomGen randomGen(randomNums, probabilities);
    }, std::invalid_argument);
}

// Test initialization with mismatched lengths
TEST(RandomGen, InitializationMismatchedLengths) {
    std::vector<int> randomNums = {1, 2, 3};
    std::vector<double> probabilities = {0.5, 0.5};
    
    EXPECT_THROW({
        RandomGen randomGen(randomNums, probabilities);
    }, std::invalid_argument);
}

// Test initialization with empty arrays
TEST(RandomGen, InitializationEmptyArrays) {
    std::vector<int> randomNums;
    std::vector<double> probabilities;
    
    EXPECT_THROW({
        RandomGen randomGen(randomNums, probabilities);
    }, std::invalid_argument);
}

// Test edge case: probability sums very close to 1 but not exactly 1
TEST(RandomGen, InitializationAlmostOne) {
    std::vector<int> randomNums = {1, 2, 3};
    std::vector<double> probabilities = {0.3, 0.3, 0.4 - 1e-12};
    
    // Should not throw due to epsilon tolerance in validation
    EXPECT_NO_THROW({
        RandomGen randomGen(randomNums, probabilities);
    });
}

// Test edge case: probability = 1 for a single value
TEST(RandomGen, SingleValueWithProbabilityOne) {
    std::vector<int> randomNums = {42};
    std::vector<double> probabilities = {1.0};
    
    RandomGen randomGen(randomNums, probabilities);
    
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(randomGen.nextNum(), 42);
    }
}

// Test edge case: uniform distribution
TEST(RandomGen, UniformDistribution) {
    const size_t numValues = 5;
    std::vector<int> randomNums;
    std::vector<double> probabilities;
    
    for (size_t i = 0; i < numValues; ++i) {
        randomNums.push_back(static_cast<int>(i));
        probabilities.push_back(1.0 / numValues);
    }
    
    RandomGen randomGen(randomNums, probabilities);
    
    std::unordered_map<int, int> results;
    constexpr int numSamples = 10000;
    
    for (int i = 0; i < numSamples; ++i) {
        int num = randomGen.nextNum();
        results[num]++;
    }
    
    // Check that each number appears with approximately equal frequency
    for (size_t i = 0; i < numValues; ++i) {
        int num = randomNums[i];
        double expectedCount = numSamples / numValues;
        int actualCount = results[num];
        
        // Allow for some statistical variation (4 standard deviations for 99.99% confidence)
        // Standard deviation for binomial distribution is sqrt(n*p*(1-p))
        double stdDev = std::sqrt(numSamples * probabilities[i] * (1 - probabilities[i]));
        double tolerance = 4 * stdDev;
        
        EXPECT_NEAR(actualCount, expectedCount, tolerance)
            << "Expected around " << expectedCount << " occurrences of " << num 
            << ", got " << actualCount;
    }
}

// Test distribution of generated numbers
TEST(RandomGen, Distribution) {
    std::vector<int> randomNums = {-1, 0, 1, 2, 3};
    std::vector<double> probabilities = {0.01, 0.3, 0.58, 0.1, 0.01};
    
    RandomGen randomGen(randomNums, probabilities);
    
    // Generate a large number of samples to ensure statistical significance
    std::unordered_map<int, int> results;
    constexpr int numSamples = 10000;
    
    for (int i = 0; i < numSamples; ++i) {
        int num = randomGen.nextNum();
        results[num]++;
    }
    
    // Check that each number appears with the expected frequency (with some tolerance)
    for (size_t i = 0; i < randomNums.size(); ++i) {
        int num = randomNums[i];
        double expectedCount = probabilities[i] * numSamples;
        int actualCount = results[num];
        
        // Allow for statistical variation
        double stdDev = std::sqrt(numSamples * probabilities[i] * (1 - probabilities[i]));
        double tolerance = 4 * stdDev;
        
        EXPECT_NEAR(actualCount, expectedCount, tolerance)
            << "Expected around " << expectedCount << " occurrences of " << num 
            << ", got " << actualCount;
    }
}

// Test thread safety
TEST(RandomGen, ThreadSafety) {
    std::vector<int> randomNums = {1, 2, 3, 4, 5};
    std::vector<double> probabilities = {0.2, 0.2, 0.2, 0.2, 0.2};
    
    RandomGen randomGen(randomNums, probabilities);
    
    // Launch multiple threads all calling nextNum() concurrently
    constexpr int numThreads = 10;
    constexpr int numCallsPerThread = 1000;
    
    auto threadFunc = [&randomGen, &numCallsPerThread]() {
        std::unordered_map<int, int> localResults;
        for (int i = 0; i < numCallsPerThread; ++i) {
            int num = randomGen.nextNum();
            localResults[num]++;
        }
        return localResults;
    };
    
    std::vector<std::future<std::unordered_map<int, int>>> futures;
    
    for (int i = 0; i < numThreads; ++i) {
        futures.push_back(std::async(std::launch::async, threadFunc));
    }
    
    // Wait for all threads and combine results
    std::unordered_map<int, int> combinedResults;
    for (auto& future : futures) {
        auto results = future.get();
        for (const auto& [num, count] : results) {
            combinedResults[num] += count;
        }
    }
    
    // Verify total number of samples
    int totalSamples = 0;
    for (const auto& [_, count] : combinedResults) {
        totalSamples += count;
    }
    EXPECT_EQ(totalSamples, numThreads * numCallsPerThread);
    
    // Check distribution
    for (size_t i = 0; i < randomNums.size(); ++i) {
        int num = randomNums[i];
        double expectedCount = probabilities[i] * totalSamples;
        int actualCount = combinedResults[num];
        
        // Allow for statistical variation
        double stdDev = std::sqrt(totalSamples * probabilities[i] * (1 - probabilities[i]));
        double tolerance = 4 * stdDev;
        
        EXPECT_NEAR(actualCount, expectedCount, tolerance)
            << "Expected around " << expectedCount << " occurrences of " << num 
            << ", got " << actualCount;
    }
}