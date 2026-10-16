#ifndef OBRC_MICHAEL_HPP
#define OBRC_MICHAEL_HPP

/*
 * References:
 * [1] - https://www.man7.org/linux/man-pages/man3/sysconf.3.html
 * [2] - https://www.man7.org/linux/man-pages/man2/mmap.2.html
 */

#include <string>

#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstring>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/time.h>

#include <immintrin.h>

typedef struct WeatherStationData {
    const char **names;    // ["London", "Tokyo", "Paris", ...]
    double **temperatures; // [[<min_0>, <mean_0>, <max_0>], ..., [<min_n-1>,
                           // <mean_n-1>, <max_n-1>]]
    uint32_t *counts;      // [200, 47, 18, 637, ...]

    uint32_t size;
    uint32_t capacity;
} WeatherStationData;

WeatherStationData *weatherStationDataCreate() {
    WeatherStationData *data = (WeatherStationData *)::malloc(sizeof(*data));
    if (!data) {
        ::printf("Error: WeatherStationData creation failed! Unable to malloc "
                 "data!\n");
        return NULL;
    }

    // Init array sizes and capacities
    data->size = 0;
    data->capacity = 1;

    // Malloc arrays to base capacity
    data->names =
        (const char **)::malloc(data->capacity * sizeof(*data->names));
    if (!data->names) {
        ::printf("Error: WeatherStationData creation failed! Unable to malloc "
                 "names array!\n");
        ::free(data);

        return NULL;
    }

    data->temperatures =
        (double **)::malloc(data->capacity * sizeof(*data->temperatures));
    if (!data->temperatures) {
        ::printf("Error: WeatherStationData creation failed! Unable to malloc "
                 "temperatures array!\n");
        ::free(data->names);
        ::free(data);

        return NULL;
    }

    data->counts = (uint32_t *)::malloc(data->capacity * sizeof(*data->counts));
    if (!data->counts) {
        ::printf("Error: WeatherStationData creation failed! Unable to malloc "
                 "counts array!\n");
        ::free(data->temperatures);
        ::free(data->names);
        ::free(data);

        return NULL;
    }

    return data;
}

bool weatherStationDataContainsName(WeatherStationData *data, const char *name,
                                    uint32_t *location) {
    for (size_t i = 0; i < data->size; ++i) {
        if (strcmp(data->names[i], name) == 0) {
            if (location) {
                *location = i;
            }

            return true;
        }
    }

    return false;
}

void weatherStationDataInsert(WeatherStationData *data, const char *name,
                              double temperature) {
    // Entry doesn't exist in list yet
    if (!weatherStationDataContainsName(data, name, NULL)) {
        if (data->size == data->capacity) {
            data->names = (const char **)::realloc(
                data->names, (data->capacity * 2) * sizeof(*data->names));
            data->temperatures = (double **)::realloc(
                data->temperatures,
                (data->capacity * 2) * sizeof(*data->temperatures));
            data->counts = (uint32_t *)::realloc(
                data->counts, (data->capacity * 2) * sizeof(*data->counts));

            data->capacity *= 2;
        }

        for (size_t i = 0; i < data->capacity; ++i) {
            if (i == data->size) {
                data->names[i] = name;

                data->temperatures[i] =
                    (double *)::malloc(3 * sizeof(**data->temperatures));
                data->temperatures[i][0] = temperature; // min
                data->temperatures[i][1] = temperature; // mean
                data->temperatures[i][2] = temperature; // max

                ++data->counts[i];

                break;
            }
        }

        ++data->size;
        return;
    }

    // Entry with name exists (at location)
    uint32_t location;
    weatherStationDataContainsName(data, name, &location);

    // Set current min if temp is less than current min
    if (data->temperatures[location][0] > temperature) {
        data->temperatures[location][0] = temperature;
    }

    // Add new temp to current rolling mean temps
    data->temperatures[location][1] += temperature;

    // Set current max to new temp if new temp exceeds current max
    if (data->temperatures[location][2] < temperature) {
        data->temperatures[location][2] = temperature;
    }

    // Update counts at name location
    ++data->counts[location];
}

void weatherStationDataParseRow(WeatherStationData *data, char *row) {
    const char *name = ::strtok_r(row, ";", &row);
    double temperature = 0.0;

    char *end; // String after double (but we have none...)
    temperature = ::strtod(row, &end);

    weatherStationDataInsert(data, name, temperature);
}

void weatherStationDataPrintResults(WeatherStationData *data) {
    for (size_t i = 0; i < data->size; ++i) {
        data->temperatures[i][1] /= data->counts[i];
        ::printf("%s;%1.f;%1.f;%1.f", data->names[i], data->temperatures[i][0],
                 data->temperatures[i][1], data->temperatures[i][2]);
    }
}

void weatherStationDataDestroy(WeatherStationData *data) {
    if (!data) {
        return;
    }

    // names and temperatures should exist if data object does... famous last
    // words?
    ::free(data->counts);
    ::free(data->temperatures);
    ::free(data->names);
    ::free(data);
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string &filename) {
    struct timeval startTime;
    ::gettimeofday(&startTime, NULL);

    ::printf("Running ");

    // Obtain OS's default page size
    const size_t pageSize = ::sysconf(_SC_PAGE_SIZE);

    // Get the file's size (in bytes)
    struct stat st;
    ::stat(filename.c_str(), &st);
    size_t fileSize = st.st_size;

    ::setlocale(LC_NUMERIC, "en_US.UTF-8");
    ::printf("(file size: %'ld MiB)...\n", fileSize / 1'000'000);

    // Open the file as a handle
    int fd = ::open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        ::perror("open");
        exit(EXIT_FAILURE);
    }

    uint32_t bufferCount = fileSize / pageSize;
    if (fileSize % pageSize) {
        ++bufferCount;
    }

    WeatherStationData *stations = weatherStationDataCreate();

    for (size_t i = 0; i < bufferCount; ++i) {
        // Map file to virtual memory avoiding explicit read/write syscalls and
        // any moving of data from user/kernel space, one "page" at a time
        void *addr =
            ::mmap(NULL, pageSize, PROT_READ, MAP_PRIVATE, fd, pageSize * i);
        if (addr == MAP_FAILED) {
            ::printf("Error: map failed!\n");
            exit(EXIT_FAILURE);
        }

        // Get a pointer to the data as a character array
        char *data = strdup((char *)addr);

        // Add null-terminator to the end of the data buffer
        data = (char *)realloc(data, (pageSize + 1) * sizeof(*data));
        data[pageSize] = '\0';

        // Parse currently buffer (4KiB)
        for (size_t j = 0; j < pageSize; j++) {
            // We have reached the end of this buffer
            if (!strcmp(data, "") == 0) {
                break;
            }

            char *row = ::strtok_r(data, "\n", &data);
            if (!row) {
                ::printf("Error: empty string passed to strtok_r()!\n");
                exit(EXIT_FAILURE);
            }

            weatherStationDataParseRow(stations, row);
        }

        ::free(data);

        // Unmap before next buffer
        if (::munmap(addr, pageSize) == -1) {
            ::printf("Error: unmap failed!\n");
            exit(EXIT_FAILURE);
        }
    }

    // print all weather station names and their min/mean/max
    weatherStationDataPrintResults(stations);

    // Cleanup
    weatherStationDataDestroy(stations);
    ::close(fd);

    struct timeval endTime;
    ::gettimeofday(&endTime, NULL);

    double msTime = (double)(endTime.tv_sec - startTime.tv_sec) / 60.0;
    ::printf("Done in %fm (%fs).\n", msTime, msTime * 60.0);
}

#endif // OBRC_MICHAEL_HPP