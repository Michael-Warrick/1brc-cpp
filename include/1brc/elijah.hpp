#ifndef ELIJAH_HPP
#define ELIJAH_HPP

#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>

#include <unistd.h>

struct StationData {
    short min_temp;
    short max_temp;
    int total_temp;
    int count;

    StationData() : min_temp(std::numeric_limits<short>::max()), 
                    max_temp(std::numeric_limits<short>::lowest()), 
                    total_temp(0), 
                    count(0) {}
};

inline short str_temp_to_int(std::string_view value) {
    bool negative = false;
    std::size_t pos = 0;

    if (value[pos] == '-') {
        negative = true;
        ++pos;
    } else if (value[pos] == '+') {
        ++pos;
    }

    short whole = 0;
    while (value[pos] != '.') {
        whole = whole * 10 + (value[pos] - '0');
        pos++;
    }

    short decimal = 0;
    ++pos;
    decimal = value[pos] - '0';

    short result = whole * 10 + decimal;
    return negative ? -result : result;
}

inline void chunk_data(std::unordered_map<std::string, StationData>& station_data, const std::string& filename) {
    const size_t BUFFER_SIZE = ::sysconf(_SC_PAGE_SIZE);
    FILE *fp;
    char* buffer = new char[BUFFER_SIZE+1];
    char* overflow = new char[32];
    overflow[0] = '\0';
    int overflow_size = 0;
    char* next;
    char* end;
    size_t result;
    fp = fopen(filename.c_str(), "rb");
    if(fp == NULL) {
        std::cerr << "Error: Failed to open file \"" << filename << "\"!\n";
        return;
    }
    while((result = fread(buffer, 1, BUFFER_SIZE - overflow_size, fp)) > 0){
        if(overflow_size > 0){
            memmove(buffer + overflow_size, buffer, result);
            memcpy(buffer, overflow, overflow_size);
            result += overflow_size;
        }
        buffer[result] = '\0';
        end = strrchr(buffer, '\n');
        for(size_t i = 0; i < end - buffer;){
            next = strchr(&buffer[i],';');

            std::string station_name(&buffer[i], next - &buffer[i]);
            //std::cout << "station name: " << station_name << std::endl;

            i += (next - &buffer[i]) + 1;
            next = strchr(&buffer[i],'\n');

            std::string_view temp_string(&buffer[i], next - &buffer[i]);

            i += (next - &buffer[i]) + 1;

            short temperature = str_temp_to_int(temp_string);

            if(!station_data.contains(station_name)){
                //std::cout << "New station: " << station_name << std::endl << std::endl;
                station_data.insert({station_name, StationData()});
            }

            if(station_data[station_name].max_temp < temperature){
                station_data[station_name].max_temp = temperature;
            }
            if(station_data[station_name].min_temp > temperature){
                station_data[station_name].min_temp = temperature;
            }
            station_data[station_name].total_temp += temperature;
            station_data[station_name].count++;
        }
        overflow_size = &buffer[result] - (end + 1);
        memcpy(overflow, end+1, overflow_size);
    }
    free(buffer);
    free(overflow);
    fclose(fp);
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string& filename) {

    std::unordered_map<std::string, StationData> station_data{};
    station_data.reserve(413);
    chunk_data(station_data, filename);
    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file \n";
        return;
    }
    std::vector<std::pair<const std::string*, const StationData*>> sorted;
    sorted.reserve(413);
    
    for (const auto& [name, data] : station_data) {
        sorted.emplace_back(&name, &data);
    }

    std::sort(sorted.begin(), sorted.end(),
              [](const auto& lhs, const auto& rhs) {
                  return *lhs.first < *rhs.first;
              });
    
    for (const auto& [station_name, data] : sorted) {
        double average = static_cast<double>(data->total_temp) / data->count / 10.0;
        outputFile << std::fixed << std::setprecision(1) << *station_name << ";" << data->min_temp << ";" << average << ";" << data->max_temp << "\n";
    }
}

#endif //ELIJAH_HPP