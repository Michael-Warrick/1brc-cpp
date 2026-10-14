#ifndef OBRC_MICHAEL_HPP
#define OBRC_MICHAEL_HPP

#include <string>
// #include <chrono>

#include <cstdio>
#include <cstdint>
#include <ctime>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/fcntl.h>

void read(const char *path, char *buffer, size_t bufferSize) {
    struct stat st;
    ::stat(path, &st);
    size_t fileSize = st.st_size;

    ::setlocale(LC_NUMERIC, "en_US.UTF-8");
    ::printf("Filesize: %'ld MiB\n", fileSize / 1'000'000);

    FILE *file = ::fopen(path, "rb");
    if (file) {
        while (::fgets(buffer, bufferSize, file)) {
            ::printf("Reading into buffer... ");
            time_t startTime = ::clock();

            size_t returnValue = ::fread(buffer, bufferSize, 1, file);

            time_t endTime = ::clock();
            time_t elapsedTime = endTime - startTime;
            uint32_t msTime = elapsedTime * 1000 / CLOCKS_PER_SEC;
            ::printf("Done (%ds %dms).\n", msTime / 1000, msTime % 1000);
        }

        ::fclose(file);
    }
}

void mmapRead(const char *path, char *buffer, size_t bufferSize) {
    struct stat st;
    ::stat(path, &st);
    size_t fileSize = st.st_size;

    ::setlocale(LC_NUMERIC, "en_US.UTF-8");
    ::printf("Filesize: %'ld MiB\n", fileSize / 1'000'000);

    ::printf("Reading with mmap... ");
    time_t startTime = ::clock();

    FILE *file = ::fopen(path, "rb");
    if (file) {
        uint32_t bufferCount = fileSize / bufferSize;
        if (fileSize % bufferSize) {
            ++bufferCount;
        }

        for (size_t i = 0; i < bufferCount; ++i) {
            buffer = (char *)mmap(NULL, bufferSize, PROT_READ,
                                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE,
                                  (long)file, i * bufferSize);
            if (buffer == MAP_FAILED) {
                ::printf("Error: map failed!\n");
                exit(EXIT_FAILURE);
            }

            if (::munmap(buffer, bufferSize) == -1) {
                ::printf("Error: unmap failed!\n");
                exit(EXIT_FAILURE);
            }
        }

        ::fclose(file);
    }

    time_t endTime = ::clock();
    time_t elapsedTime = endTime - startTime;
    uint32_t msTime = elapsedTime * 1000 / CLOCKS_PER_SEC;
    ::printf("Done (%ds %dms).\n", msTime / 1000, msTime % 1000);
}

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string &filename) {
    ::printf("Running...\n");

    // Create a compile-time buffer of 1GiB
    // constexpr size_t bufferSize = 1024 * 1024 * 1024;
    // char *buffer = (char *)::malloc(bufferSize * sizeof(*buffer));

    const size_t pageSize = ::sysconf(_SC_PAGE_SIZE);
    printf("Page size: %ld bytes\n", pageSize);
    char *pageBuffer = (char *)::malloc(pageSize * sizeof(*pageBuffer));

    // read(filename.c_str(), buffer, bufferSize);
    mmapRead(filename.c_str(), pageBuffer, pageSize);

    ::free(pageBuffer);
    ::printf("Done.\n");
}

#endif // OBRC_MICHAEL_HPP