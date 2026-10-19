#ifndef OBRC_USERNAME_HPP
#define OBRC_USERNAME_HPP

#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

#include <sys/mman.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>

// constexpr std::size_t CHUNK_SIZE = 512 * 1024 * 1024;

using StationID = std::uint16_t;
using Temperature = std::int32_t;

// class BufferedReader {
//     private:
//         char* buffer_;
//         std::string overflow_;
//         FILE* file_;
            
//     public:
//         BufferedReader(std::string_view filename) : buffer_(new char[CHUNK_SIZE]), file_(fopen(filename.data(), "rb")) {
//             if (file_ == NULL) std::terminate();
//         };

//         char* read(size_t& bytes_read) {
//             bytes_read = std::fread(buffer_, 1, CHUNK_SIZE, file_);
            
//             return buffer_;
//             // char* of = std::strrchr(buffer_, '\n');
//             // if (of == NULL) std::cout << "null\n";
//             // else std::cout << "notnull\n" << of << "\n";
//         }
// };

struct StringViewHash {
    using is_transparent = void;

    std::size_t operator()(const std::string_view& sv) const {
        return std::hash<std::string_view>{}(sv);
    }
};

struct StringViewEquals {
    using is_transparent = void;

    bool operator()(const std::string_view& a, const std::string_view& b) const {
        return (a == b);
    }
};

Temperature stotemp(const char* ptr) {
    bool negative = (*ptr == '-');

    if (negative) ++ptr;

    Temperature value{0};

    if (ptr[1] == '.') {
        // x.x
        value = static_cast<Temperature>((ptr[0] - '0') * 10 + (ptr[2] - '0'));

        return negative ? static_cast<Temperature>(-value) : static_cast<Temperature>(value);
    }

    // xx.x

    value = static_cast<Temperature>((ptr[0] - '0') * 100 + (ptr[1] - '0') * 10 + (ptr[3] - '0'));

    return negative ? static_cast<Temperature>(-value) : static_cast<Temperature>(value);
}

std::string temptos(const Temperature& temperature) {
    const std::int64_t value = temperature;
    const std::int64_t magnitude = value < 0 ? -value : value;

    return (value < 0 ? "-" : "") +
           std::to_string(magnitude / 10) + "." +
           std::to_string(magnitude % 10);
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string& filename) {
    struct Measurement {
        std::int64_t sum = 0;
        size_t count = 0;
        Temperature min = std::numeric_limits<Temperature>::max();
        Temperature max = std::numeric_limits<Temperature>::min();
    };

    struct Station {
        std::string station_;
        Measurement measurement_;
    };
    
    std::unordered_map<std::string_view, StationID, StringViewHash, StringViewEquals> name_lookup{};
    std::vector<Station> data{};
    
    int fd = open(filename.c_str(), O_RDONLY);
    size_t filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char* mapped = static_cast<char*>(
        mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0)
    );

    const char* file_ptr = mapped;
    const char* const end_ptr = file_ptr + filesize;

    while (file_ptr < end_ptr) {
        const char* semi = static_cast<const char*>(std::memchr(file_ptr, ';', end_ptr - file_ptr));
        const char* newline = static_cast<const char*>(std::memchr(semi + 1, '\n', end_ptr - semi - 1));

        std::string_view station_name = std::string_view(file_ptr, semi - file_ptr);
        Temperature temp = stotemp(semi + 1);

        // std::cout << temp_string << " " << temp << "\n";

        // std::cout << (temp == temp2) << "\n";

        auto [it, inserted] = name_lookup.try_emplace(
            station_name,
            static_cast<StationID>(data.size())
        );

        if (inserted) {
            data.push_back(
                {std::string(station_name), {}}
            );
        }

        Measurement& m = data[it->second].measurement_;

        m.sum += temp;
        m.count++;

        if (temp < m.min) {
            m.min = temp;
        }

        if (temp > m.max) {
            m.max = temp;
        }

        file_ptr = newline + 1;
    }

    munmap(mapped, filesize);
    close(fd);

    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file \n";
        return;
    }

    std::sort(
        data.begin(),
        data.end(),
        [] (const Station& a, const Station& b) { return a.station_ < b.station_; }
    );

    for (const auto& station: data) {
        double average = (station.measurement_.sum / 10.0) / station.measurement_.count;
        outputFile << std::fixed << std::setprecision(1) << station.station_ << ";" << temptos(station.measurement_.min) << ";" << average << ";" << temptos(station.measurement_.max) << "\n";
    }
    
}

#endif //OBRC_USERNAME_HPP