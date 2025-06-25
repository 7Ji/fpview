#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[]) {
    struct option const long_options[] = {
        {"help",            no_argument,        NULL,   'h'},
        {"float",           no_argument,        NULL,   'f'},
        {"single",          no_argument,        NULL,   's'},
        {"double",          no_argument,        NULL,   'd'},
        {"both",            no_argument,        NULL,   'b'},
        {NULL,              0,                  NULL,  '\0'}
    };
    int c, option_index;
    enum FloatingFormat : char {
        FloatingFormatBoth = 0b11,
        FloatingFormatSingle = 0b01,
        FloatingFormatDouble = 0b10,
    } format = FloatingFormatBoth;
    while ((c = getopt_long(argc, argv, "hfsdb", long_options, &option_index)) != -1) {
        switch (c) {
        case 'h':
            puts("Not written yet");
            return 0;
        case 'f':
            format = FloatingFormatSingle;
            break;
        case 's':
            format = FloatingFormatSingle;
            break;
        case 'd':
            format = FloatingFormatDouble;
            break;
        case 'b':
            format = FloatingFormatBoth;
            break;
        default:
            printf("Unknown option '%s'", argv[optind - 1]);
            return 1;
        }
    }
    puts("Parts are colored:\n\033[34m sign\033[32m exponent\033[31m fraction\033[0m");
    for (int i = optind; i < argc; ++i) {
        if (format & FloatingFormatSingle) {
            printf("%s as single-precision:\n", argv[i]);
            union {
                float number;
                uint32_t bytes;
            } overlap;
            char *end;
            overlap.number = strtof(argv[i], &end);
            if (overlap.number == 0.0 && end == argv[i]) {
                puts("------ BAD INPUT ------");
                continue;
            }
            puts("\033[34m 31\033[32m 30 29 28 27 26 25 24 23\033[31m 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0\033[0m");
            printf("  %hhu", (uint8_t)(overlap.bytes >> 31) & 0b1);
            for (short i = 30; i >= 23; --i) {
                printf("  %hhu", (uint8_t)(overlap.bytes >> i) & 0b1);
            }
            for (short i = 22; i >= 0; --i) {
                printf("  %hhu", (uint8_t)(overlap.bytes >> i) & 0b1);
            }
            printf(" \n");
        }
        if (format & FloatingFormatDouble) {
            printf("%s as double-precision:\n", argv[i]);
            union {
                double number;
                uint64_t bytes;
            } overlap;
            char *end;
            overlap.number = strtod(argv[i], &end);
            if (overlap.number == 0.0 && end == argv[i]) {
                puts("------ BAD INPUT ------");
                continue;
            }
            puts("\033[34m 63\033[32m 62 61 60 59 58 57 56 55 54 53 52\033[31m 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0\033[0m");
            printf("  %hhu", (uint8_t)(overlap.bytes >> 63) & 0b1);
            for (short i = 62; i >= 52; --i) {
                printf("  %hhu", (uint8_t)(overlap.bytes >> i) & 0b1);
            }
            for (short i = 51; i >= 0; --i) {
                printf("  %hhu", (uint8_t)(overlap.bytes >> i) & 0b1);
            }
            printf(" \n");
        }
    }

    return 0;
}
