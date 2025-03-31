#include "RandomGen.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>

int main() {
    try {
        std::vector<int> randomNums = {-1, 0, 1, 2, 3};
        std::vector<double> probabilities = {0.01, 0.3, 0.58, 0.1, 0.01};
        
        std::cout << "Initializing RandomGen with:" << std::endl;
        std::cout << "Numbers: ";
        for (const auto& num : randomNums) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
        
        std::cout << "Probabilities: ";
        for (const auto& prob : probabilities) {
            std::cout << prob << " ";
        }
        std::cout << std::endl << std::endl;
        
        // Create the RandomGen instance
        RandomGen randomGen(randomNums, probabilities);
        
        // Generate 100 random numbers and count their occurrences
        std::unordered_map<int, int> results;
        constexpr int numSamples = 100;
        
        for (int i = 0; i < numSamples; ++i) {
            int num = randomGen.nextNum();
            results[num]++;
        }
        
        std::cout << "Results after " << numSamples << " calls to nextNum():" << std::endl;
        std::cout << std::setw(5) << "Value" << std::setw(12) << "Count" << std::setw(15) << "Expected" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        
        for (const auto& num : randomNums) {
            double expectedCount = 0;
            for (size_t i = 0; i < randomNums.size(); ++i) {
                if (randomNums[i] == num) {
                    expectedCount = probabilities[i] * numSamples;
                    break;
                }
            }
            
            std::cout << std::setw(5) << num << std::setw(12) << results[num] 
                      << std::setw(15) << std::fixed << std::setprecision(1) << expectedCount << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}