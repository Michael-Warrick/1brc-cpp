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

// Don't change signature of this function, feel free to change everything else
inline void solution(const std::string &filename) {
    time_t startTime = ::clock();
    ::printf("Running ");

    // Obtain OS's default page size
    const size_t bufferSize = ::sysconf(_SC_PAGE_SIZE);
    char *buffer = (char *)::malloc(bufferSize * sizeof(*buffer));

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

    uint32_t bufferCount = fileSize / bufferSize;
    if (fileSize % bufferSize) {
        ++bufferCount;
    }

    for (size_t i = 0; i < bufferCount; ++i) {
        // Map file to virtual memory avoiding explicit read/write syscalls and
        // any moving of data from user/kernel space
        void *addr = ::mmap(NULL, bufferSize, PROT_READ, MAP_PRIVATE, fd,
                            bufferSize * i);
        if (addr == MAP_FAILED) {
            ::printf("Error: map failed!\n");
            exit(EXIT_FAILURE);
        }

        // Get a pointer to the data as a character array
        char *data = (char *)addr;

        // Parse currently buffer (4KiB)
        for (size_t j = 0; j < bufferSize; j++) {
            // Ignore ';' and numeric values
            if (data[j] == ';' || ::isdigit(data[j])) {
                continue;
            }

            // Ignore '.' characters only if decimal point, not an
            // abbreviation (i.e., St. John's)
            if (data[j] == '.' && ::isdigit(data[j + 1])) {
                continue;
            }

            // Ignore '-' characters only if it is an integer negation, not a
            // hyphenation (i.e., Port-Gentil)
            if (data[j] == '-' && ::isdigit(data[j + 1])) {
                continue;
            }

            // printf("%c", data[j]);
        }

        // Unmap before next buffer
        if (::munmap(addr, bufferSize) == -1) {
            ::printf("Error: unmap failed!\n");
            exit(EXIT_FAILURE);
        }
    }

    ::close(fd);
    ::free(buffer);

    time_t endTime = ::clock();
    time_t elapsedTime = endTime - startTime;
    uint32_t msTime = elapsedTime * 1000 / CLOCKS_PER_SEC;
    ::printf("Done (%dm %ds %dms).\n", (msTime / 1000) / 60, (msTime / 1000) % 60,
             msTime % 1000);
}

#endif // OBRC_MICHAEL_HPP