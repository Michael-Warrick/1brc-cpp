#include "1brc-generator/generator.hpp"

#include <random>
#include <cmath>
#include <cstring>
#include <iostream>
#include <chrono>

#include "1brc-generator/stations.hpp"

namespace obrc {
Generator::Generator() {
    std::cout << "Chunk Generation Starting...\n";
    const auto startTime = std::chrono::high_resolution_clock::now();
    m_weatherStationData.resize(m_kChunkSize);

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> integerDistribution(
        0, kWeatherStations.size() - 1);
    std::uniform_int_distribution<> temperatureDistribution(-99, 399);

    for (uint32_t i = 0; i < m_kChunkSize; ++i) {
        const char *stationName =
            kWeatherStations.at(integerDistribution(generator));
        float temperature =
            convertFixedPrecisionToFloat(temperatureDistribution(generator), 1);

        std::string entryData;
        entryData.append(stationName);
        entryData.append(";");
        entryData.append(toStringWithPrecision(temperature, 1));

        m_weatherStationData.at(i) = entryData.c_str();
    }

    const auto endTime = std::chrono::high_resolution_clock::now();
    auto millisecondsAsInt = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::chrono::duration<double, std::milli> millisecondsAsDouble = endTime - startTime;

    std::cout << "Chunk Generation Completed! Time taken: " << millisecondsAsInt.count() << "ms\n";
}

Generator::~Generator() {}

int Generator::convertFloatToFixedPrecision(float value,
                                            int fractionalDigitCount) {
    return static_cast<int>(
        std::roundf(value * powf(10.0f, fractionalDigitCount)));
}

float Generator::convertFixedPrecisionToFloat(int value,
                                              int fractionalDigitCount) {
    return static_cast<float>(value) / powf(10.0f, fractionalDigitCount);
}

} // namespace obrc
