#ifndef OBRC_USERNAME_HPP
#define OBRC_USERNAME_HPP

#include <cmath>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

static constexpr std::uint64_t CHUNK_SIZE = 64 * 1024 * 1024;

using StationID = std::uint16_t;
using Temperature = std::int16_t;

Temperature stotemp(const std::string& str) {
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
    
    while (i < size) {
        if (str[i] == '.') { ++i; continue; }

        out += (str[i] - '0') * ( std::pow(10, size - i - 1) );
        ++i;
    }

    out += (str[i] - '0');

    if (neg) out *= -1;

    return out;
}

std::string temptos(const Temperature& temperature) {
    const std::int32_t value = temperature;
    const std::int32_t magnitude = value < 0 ? -value : value;

    return (value < 0 ? "-" : "") +
           std::to_string(magnitude / 10) + "." +
           std::to_string(magnitude % 10);
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string& filename) {
    struct Measurement {
        std::uint64_t sum = 0;
        size_t count = 0;
        Temperature min = std::numeric_limits<Temperature>::max();
        Temperature max = std::numeric_limits<Temperature>::min();
    };
    
    std::unordered_map<std::string, StationID> name_lookup{};
    std::unordered_map<StationID, Measurement> station_data{};

    std::vector<std::string> keys{};
    
    StationID current_id = 0;
    

    std::ifstream inputFile(filename, std::ios::in);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file \n";
        return;
    }
    
    std::string line;
    
    while (std::getline(inputFile, line)) {
        size_t delimiter_pos = line.find(';');

        if (delimiter_pos == std::string::npos) {
            break;
        }

        std::string station_name = line.substr(0, delimiter_pos);
        StationID station_id = 0;

        if (name_lookup.contains(station_name)) {
            station_id = name_lookup[station_name];
        } else {
            name_lookup[station_name] = current_id;
            station_id = current_id++;

            keys.push_back(station_name);
        }

        Temperature temp = stotemp(line.substr(delimiter_pos + 1));

        Measurement& m = station_data[station_id];

        m.sum += temp;
        m.count++;

        if (temp < m.min) {
            m.min = temp;
        }

        if (temp > m.max) {
            m.max = temp;
        }
    }

    inputFile.close();

    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file \n";
        return;
    }

    std::sort(keys.begin(), keys.end());

    for (const auto& station: keys) {
        float sum = 0.0f;
        const auto& station_id = name_lookup[station];
        
        Measurement& measurement = station_data[station_id];

        float average = measurement.sum / measurement.count;
        outputFile << std::fixed << std::setprecision(1) << station << ";" << temptos(measurement.min) << ";" << average << ";" << temptos(measurement.max) << "\n";
    }
}

#endif //OBRC_USERNAME_HPP