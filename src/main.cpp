#include <iostream>
#include <chrono>

#include "1brc/michael.hpp"
#include "1brc/elijah.hpp"

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!\n";
    auto start = std::chrono::high_resolution_clock::now();
    solution("measurements.txt");
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms\n";
    return 0;
}