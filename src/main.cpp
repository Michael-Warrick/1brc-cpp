#include <chrono>
#include <iostream>

#include "1brc/baldeep-gill.hpp"

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    
    solution("");

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Elapsed time: " << elapsed_time.count() << " ms\n";

    return 0;
}