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

        float temperature = std::stof(line.substr(delimiter_pos + 1));
        Temperature temp = static_cast<Temperature>( temperature * 10 );

        station_data[station_id].records.push_back(temp);

        if (temp < station_data[station_id].min) {
            station_data[station_id].min = temp;
        }

        if (temp > station_data[station_id].max) {
            station_data[station_id].max = temp;
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
        const auto& vec_temp = station_data[station_id].records;
        for (Temperature temp: vec_temp) {
            sum += temp;
        }
        float average = sum / vec_temp.size();
        outputFile << std::fixed << std::setprecision(1) << station << ";" << station_data[station_id].min << ";" << average << ";" << station_data[station_id].max << "\n";
    }
}

#endif //OBRC_USERNAME_HPP