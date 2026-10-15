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

Temperature stotemp(const std::string_view& str) {
    size_t size = str.size();
    std::uint8_t i{0};
    bool neg = false;
    Temperature out{0};

    if (str[0] == '-') {
        neg = true;
        ++i;
    }
    
    --size;
    // 1 2 . 7 -> 127
    // 0 1 2 3 
    // 1 2 3 4 
    
    while (i < size && str[i] != '.') {

        out = out * 10  + (str[i] - '0');
        ++i;
    }

    if (str[i] == '.') ++i;

    out += out * 10 + (str[i] - '0');

    return (neg) ? static_cast<Temperature>(-out) : out;
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

    // junk
    // std::unordered_map<StationID, Measurement> station_data{};
    // std::vector<std::string> keys{};
    
    // StationID current_id = 0;

    //// mmap
    
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
        std::string_view temp_string = std::string_view(semi + 1, newline - semi - 1);

        Temperature temp = stotemp(temp_string);

        auto [it, inserted] = name_lookup.try_emplace(
            std::string(station_name),
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

    // for (size_t index_ = 0; index_ < filesize; ) {
    //     const char* semi = std::strchr(&mapped[index_], ';');
    //     size_t size = (semi - &mapped[index_]) + 1;
    //     // char* station_name = new char[size];
    //     std::string_view station_name(semi - &mapped[index_]);
    //     std::memcpy(station_name, &mapped[index_], size - 1);
    //     station_name[size - 1] = '\0';

    //     // std::cout << station_name << "\n";
        
    //     index_ += size;

    //     char* newline = std::strchr(&mapped[index_], '\n');
    //     size = (newline - &mapped[index_]) + 1;
    //     char* temp_str = new char[size];
    //     std::memcpy(temp_str, &mapped[index_], size - 1);
    //     temp_str[size - 1] = '\0';
    //     Temperature temp = stotemp(temp_str);

    //     index_ += size;

    //     // std::cout << station_name << temp_str << "\n";


    //     StationID station_id = 0;

    //     // std::cout << station_name << "\n";

    //     if (name_lookup.contains(station_name)) {
    //         station_id = name_lookup[station_name];
    //     } else {
    //         name_lookup[station_name] = current_id;
    //         station_id = current_id++;

    //         keys.push_back(station_name);
    //     }

    //     Measurement& m = station_data[station_id];

    //     m.sum += temp;
    //     m.count++;

    //     if (temp < m.min) {
    //         m.min = temp;
    //     }

    //     if (temp > m.max) {
    //         m.max = temp;
    //     }

    //     delete station_name;
    //     delete temp_str;
    // }

    munmap(mapped, filesize);
    close(fd);

    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file \n";
        return;
    }

    // std::sort(keys.begin(), keys.end());

    // for (const auto& station: keys) {
    //     // std::cout << station << "\n";
    //     const auto& station_id = name_lookup[station];
        
    //     Measurement& measurement = station_data[station_id];

    //     double average =  ( measurement.sum / 10.0f ) / measurement.count;
    //     outputFile << std::fixed << std::setprecision(1) << station << ";" << temptos(measurement.min) << ";" << average << ";" << temptos(measurement.max) << "\n";
    // }

    std::sort(
        data.begin(),
        data.end(),
        [] (const Station& a, const Station& b) { return a.station_ < b.station_; }
    );

    for (const auto& station: data) {
        double average = (station.measurement_.sum / 10.0) / station.measurement_.count;
        outputFile << std::fixed << std::setprecision(1) << station.station_ << ";" << temptos(station.measurement_.min) << ";" << average << temptos(station.measurement_.max) << "\n";
    }
    
}

#endif //OBRC_USERNAME_HPP