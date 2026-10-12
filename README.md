# 1brc-generator
A small program to generate the [One Billion Row Challenge](https://1brc.dev/) data.

## Introduction
> *The One Billion Row Challenge (1BRC) is a fun exploration of how far modern Java can be pushed for aggregating one billion rows from a text file. Grab all your (virtual) threads, reach out to SIMD, optimize your GC, or pull any other trick, and create the fastest implementation for solving this task!*

In this variant of the challenge, the implementation must be written in C++. For more details please consult the [language rules](#language) section.

## Rules and Limits
### Machine
The solution must compile and be runnable on the Free Tier GitHub Codespace VM:

| Component | Specification                            |
|-----------|------------------------------------------|
| CPU       | Intel Xeon Platinum 8370C (2) @ 2.800GHz |
| GPU       | N/A                                      |
| RAM       | 8GB                                      |
| OS        | Ubuntu 24.04.5 LTS x86_64                |

### Language
The solution must be written in C++. It can be written in any standard version of GNU GCC compliant C++, ranging from C++98 to C++20.

### Implementation
The solution must be written by a single person, the person participating in the challenge cannot ask someone else to write it for them (artificial nor human). 

It is against the rules to use any third-party libraries, this includes copying and pasting the underlying implementation. It is acceptable however use the Standard Template Library (STL).

The computation must happen during runtime, it is against the rules to process the `measurements.txt` file at compile time. This includes just fetching the processed results as a web request.

### Input Format
**Example:** `Ho Chi Minh City;35.7\n`

- `<station_name>`: A non-null UTF-8 string with a minimum length of 1 and maximum length of 100 bytes (whitespaces are included)
- `;`: a semicolon delimiter.
- `<temperature>`: A non-null double between -9.9 (inclusive) and 39.9 (inclusive), always one fractional digit.
- `\n`: A newline character is present at the end of every row, separating each entry.

There are 413 unique station names, from which the one billion rows are generated. Solutions should not rely on any specifics of a given data set, as any valid station name as per the above listed constraints and any data distribution (number of measurements per station) must be supported.

## Submission
1. Create a fork of this repository on your own GitHub profile. This will let you submit your solution via a pull request.

2. Create a new implementation file under `src/solutions/` with the name `<username>.cpp` (in my case it would be `michael.cpp`). To get started, it is recommended to use the reference solution as a template, however please feel free to start from scratch if you'd like.

3. Make it fast! Literally, as fast as you possibly can without running out of memory. It is recommended that you test your solution at least once on the GitHub Codespace VM before submitting as it will be the test bench for this challenge.

4. Once ready, create a pull request against the upstream repository.

5. After the submission deadline has passed, each solution will be timed and validated and a winner will be announced.

## Prize
Once a winner has been identified, they will be duely rewarded with a Terry's Chocolate Orange. There are sadly no runner-up prizes as the challenge is an "all or nothing" challenge.

## Credit
- Heavily inspired by the original challenge's [CreateMeasurementsFast.java](https://github.com/gunnarmorling/1brc/blob/main/src/main/java/dev/morling/onebrc/CreateMeasurementsFast.java) Java implementation.

- C++ variant idea by `baldeep-gill`