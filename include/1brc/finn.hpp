#ifndef OBRC_USERNAME_HPP
#define OBRC_USERNAME_HPP

#include <fstream>
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <charconv>


#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

struct station{
    int min = 1000;
    int max = -1000;
    int sum = 0;
    int elements = 0;
};

using uMap = std::unordered_map<std::string_view, station>;

inline void processChunk(
    const char* ptr,
    const char* end,
    uMap& results) 
    {
    while (ptr < end) {
        const char* nameStart = ptr;

        while (ptr < end && *ptr != ';') {
            ptr++;
        }

        if (ptr == end) {
            std::cerr << "malformed input\n";
            break;
        }

        std::string_view station_name(nameStart, ptr - nameStart);
        ptr++; // pass semicolon
        bool negative = false;

        if (ptr < end && *ptr == '-'){
            negative = true;
            ptr++;
        }

        int temperature = 0;

        while (ptr < end && *ptr != '\n' && *ptr != '\r') {
            if (*ptr != '.') {
                temperature = temperature * 10 + (*ptr - '0');
            }
            ptr++;
        }

        if (negative) {
            temperature = -temperature;
        }

        // skip newline
        if (ptr < end && *ptr == '\n') {
            ptr++;
        }

        auto it = results.find(station_name);
        if (it == results.end()){
            it = results.try_emplace(station_name).first;
        }
        
        station& curStation = it->second;

        curStation.elements++;

        if (temperature < curStation.min) {
            curStation.min = temperature;
        }
        if (temperature > curStation.max) {
            curStation.max = temperature;
        }

        curStation.sum += temperature;
    }
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);

    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

    struct stat info{};

    if (fstat(fd, &info) == -1) {
        std::perror("fstat");
        close(fd);
        return;
    }

    size_t fileSize = static_cast<size_t>(info.st_size);

    void* mapped = mmap(
        nullptr,
        fileSize,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0
    );

    close(fd); // don't need file anymore

    if (madvise(mapped, fileSize, MADV_SEQUENTIAL) == -1) {
        perror("madvise");
    }

    if (mapped == MAP_FAILED) {
        std::perror("mmap");
        return;
    }

    uMap stations1{};
    uMap stations2{};
    stations1.max_load_factor(0.5f);
    stations2.max_load_factor(0.5f);
    stations1.reserve(413);
    stations2.reserve(413);
    std::vector<std::string_view> keys;
    
    const char* begin = static_cast<const char*>(mapped);
    const char* end = begin + fileSize;

    const char* middle = begin + fileSize / 2;

    while (middle < end && *middle != '\n') {
        middle++;
    }

    // multithreaded -- sometimes slower for 1b but oh well
    std::jthread worker([&](){
        processChunk(middle, end, stations2);
    });

    processChunk(begin, middle, stations1);

    worker.join();
    
    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file \n";
        return;
    }

    keys.reserve(stations1.size());
    // Merge station maps
    for (auto& it : stations1) {
        station& cStation2 = stations2[it.first];
        station& cStation = stations1[it.first];
        keys.push_back(it.first);
        cStation.elements += cStation2.elements;
        cStation.min = std::min(cStation.min, cStation2.min);
        cStation.max = std::max(cStation.max, cStation2.max);
        cStation.sum += cStation2.sum;
    }

    std::sort (keys.begin(), keys.end());

    for (const auto& name : keys) {
        station& currentStation = stations1[name];
        float average = (currentStation.sum / 10.0f) / currentStation.elements;
        float min = currentStation.min / 10.0f;
        float max = currentStation.max / 10.0f;
        outputFile << std::fixed << std::setprecision(1) << name << ";" << min << ";" << average << ";" << max << "\n";
    }

    munmap(mapped, fileSize);
}
#endif