#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "RandomGen.h"
#include <unordered_map>
#include <memory>
#include <thread>
#include <future>
#include <vector>

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

// Test custom seeding ensures deterministic output
TEST(RandomGen, CustomSeedDeterministicOutput) {
    std::vector<int> randomNums = {-1, 0, 1, 2, 3};
    std::vector<double> probabilities = {0.01, 0.3, 0.58, 0.1, 0.01};
    
    // Create first sequence with specific seed
    RandomGen::setSeed(12345);
    RandomGen randomGen1(randomNums, probabilities);
    std::vector<int> sequence1;
    for (int i = 0; i < 100; ++i) {
        sequence1.push_back(randomGen1.nextNum());
    }
    
    // Reset with the same seed and create a second sequence
    RandomGen::setSeed(12345);
    RandomGen randomGen2(randomNums, probabilities);
    std::vector<int> sequence2;
    for (int i = 0; i < 100; ++i) {
        sequence2.push_back(randomGen2.nextNum());
    }
    
    // Both sequences should be identical
    EXPECT_EQ(sequence1, sequence2);
    
    // Try a different seed to ensure we get a different sequence
    RandomGen::setSeed(54321);
    RandomGen randomGen3(randomNums, probabilities);
    std::vector<int> sequence3;
    for (int i = 0; i < 100; ++i) {
        sequence3.push_back(randomGen3.nextNum());
    }
    
    // This sequence should differ from the first two
    EXPECT_NE(sequence1, sequence3);
}

// Test that seeding propagates across threads
TEST(RandomGen, SeedPropagationAcrossThreads) {
    std::vector<int> randomNums = {-1, 0, 1, 2, 3};
    std::vector<double> probabilities = {0.01, 0.3, 0.58, 0.1, 0.01};
    
    // Set a fixed seed
    const unsigned int SEED = 42;
    RandomGen::setSeed(SEED);
    
    // Function to generate numbers in a thread
    auto generateNumbers = [&randomNums, &probabilities](int count) {
        RandomGen gen(randomNums, probabilities);
        std::vector<int> numbers;
        for (int i = 0; i < count; ++i) {
            numbers.push_back(gen.nextNum());
        }
        return numbers;
    };
    
    // Run in main thread
    std::vector<int> mainThreadNumbers = generateNumbers(10);
    
    // Reset seed to the same value
    RandomGen::setSeed(SEED);
    
    // Run in a separate thread
    auto future = std::async(std::launch::async, [&generateNumbers]() {
        return generateNumbers(10);
    });
    
    std::vector<int> threadNumbers = future.get();
    
    // Both sequences should be identical despite running in different threads
    EXPECT_EQ(mainThreadNumbers, threadNumbers);
}