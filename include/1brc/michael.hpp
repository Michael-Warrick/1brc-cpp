#ifndef OBRC_MICHAEL_HPP
#define OBRC_MICHAEL_HPP

#include <string>
#include <fstream>
#include <memory>
#include <chrono>

#include <cstdio>

void ifstreamRead(const std::string &filename, char *buffer,
                  size_t bufferSize) {
    // Open the file
    std::ifstream measurementsFile(filename);

    while (measurementsFile) {
        std::cout << "Reading into buffer...";
        const auto bufferReadStartTime =
            std::chrono::high_resolution_clock::now();

        measurementsFile.read(buffer, bufferSize);

        // Perform computation here

        const auto bufferReadEndTime =
            std::chrono::high_resolution_clock::now();
        const auto bufferReadElapsedTime =
            std::chrono::duration_cast<std::chrono::seconds>(
                bufferReadEndTime - bufferReadStartTime);

        std::cout << "Done (" << bufferReadElapsedTime << ").\n";
    }

    measurementsFile.close();
}

void fopenRead(const char *path, void *buffer, size_t bufferSize) {
    FILE *file = fopen(path, "rb");
    if (file) {
        uint8_t chunkCount = 13;
        for (size_t i = 0; i < chunkCount; ++i) {
            std::cout << "Reading into buffer...";
            const auto bufferReadStartTime =
                std::chrono::high_resolution_clock::now();

            size_t returnValue = fread(buffer, bufferSize, 1, file);

            const auto bufferReadEndTime =
                std::chrono::high_resolution_clock::now();
            const auto bufferReadElapsedTime =
                std::chrono::duration_cast<std::chrono::seconds>(
                    bufferReadEndTime - bufferReadStartTime);

            std::cout << "Done (" << bufferReadElapsedTime << ").\n";
        }

        fclose(file);
    }
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string &filename) {
    // Define a compile-time known buffer size of 1GB
    constexpr size_t bufferSize = 1024 * 1024 * 1024;

    unsigned char *buffer = new unsigned char[bufferSize];
    // Create the character array buffer
    // std::unique_ptr<char[]> buffer(new char[bufferSize]);

    fopenRead(filename.c_str(), buffer, bufferSize);

    delete buffer;
}

#endif // OBRC_MICHAEL_HPP