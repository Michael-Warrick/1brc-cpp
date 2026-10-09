#ifndef OBRC_GENERATOR_HPP
#define OBRC_GENERATOR_HPP

#include <vector>
#include <sstream>

#include <cstdint>

namespace obrc {
class Generator {
public:
    Generator(/* args */);
    ~Generator();

private:
    int convertFloatToFixedPrecision(float value, int fractionalDigitCount);
    float convertFixedPrecisionToFloat(int value, int fractionalDigitCount);

    template <typename T>
    std::string toStringWithPrecision(const T value, const int precision = 6) {
        std::ostringstream oss;
        oss.precision(precision);
        oss << std::fixed << value;

        return std::move(oss).str();
    }

    // Cannot expect to store 8 GB of data in memory on an 8 GB machine
    // so divide data into 2 GB chunks.

    // Assuming the worst case, an entry could have:
    // * city_name: 23 bytes
    // * ';': 1 byte
    // * temperature: (+/-) <whole_number>.<fractional_part> 4 bytes
    // * null-terminator ('\0'): 1 byte
    //
    // In-memory, for each entry, we store a `const char *` and on a 64-bit
    // machine is most likely 8 bytes.
    //
    // Total: 29 + 8 = 37 bytes
    //
    // Therefore (in the worst case), 1,000,000,000 entries would amount to
    // 37,000,000,000 bytes or 37 GB.
    //
    // Given the codespace machines only have 8 GB of system memory, it would be
    // wise to chunk the data ensuring it is more manageable.
    //
    // Hence, we can take a factor of 8 (either 2 or maybe 4) and use it for our
    // hard memory limit when chunking.
    //
    // Choosing 2 GB as a baseline and knowing that for 1,000,000,000 entries,
    // our worst case memory footprint would be of 37 GB, we can simply
    // calculate that for 2 GB we would arrive at 54,054,054.0541 (2 *
    // 1,000,000,000 / 37) and to be on the safe side, we'll round down to the
    // nearest value: 54,054,054.
    //
    // We could also round down to the nearest power of 2: 33,554,432 (2^25)...
    static constexpr uint32_t m_kChunkSize = 54'054'054;
    std::vector<const char *> m_weatherStationData;
};

} // namespace obrc

#endif // OBRC_GENERATOR_HPP