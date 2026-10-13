#ifndef OBRC_USERNAME_HPP
#define OBRC_USERNAME_HPP

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string& filename) {
    using StationID = std::uint16_t;
    using Temperature = std::int16_t;

    struct Measurement {
        std::vector<Temperature> records{};
        Temperature min = std::numeric_limits<Temperature>::max();
        Temperature max = std::numeric_limits<Temperature>::min();
    };

    StationID current_id = 0;
    
    std::map<std::string, StationID, std::less<std::string>> name_lookup{};
    
    std::unordered_map<StationID, std::vector<Temperature>> station_data{};
    
    std::unordered_map<StationID, std::pair<Temperature, Temperature>> station_ranges{};
    
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
            station_ranges[station_id] = std::pair<Temperature, Temperature>{std::numeric_limits<Temperature>::max(), std::numeric_limits<Temperature>::min()};
        }

        float temperature = std::stof(line.substr(delimiter_pos + 1));
        Temperature temp = static_cast<Temperature>( temperature * 10 );
        station_data[station_id].push_back(temp);

        if (temp < station_ranges[station_id].first) {
            station_ranges[station_id].first = temp;
        }

        if (temp > station_ranges[station_id].second) {
            station_ranges[station_id].second = temp;
        }
    }

    inputFile.close();

    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file \n";
        return;
    }

    for (const auto& [station_name, station_id] : name_lookup) {
        float sum = 0.0f;
        const auto& vec_temp = station_data[station_id];
        for (Temperature temp: vec_temp) {
            sum += temp;
        }
        float average = sum / vec_temp.size();
        outputFile << std::fixed << std::setprecision(1) << station_name << ";" << station_ranges[station_id].first << ";" << average << ";" << station_ranges[station_id].second << "\n";
    }
}

#endif //OBRC_USERNAME_HPP