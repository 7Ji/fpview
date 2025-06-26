#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>

#define SINGLE_OFF_FRACTION_LOW      0
#define SINGLE_OFF_FRACTION_HIGH    22
#define SINGLE_OFF_EXPONENT_LOW     23
#define SINGLE_OFF_EXPONENT_HIGH    30
#define SINGLE_OFF_SIGN             31

#define DOUBLE_OFF_FRACTION_LOW      0
#define DOUBLE_OFF_FRACTION_HIGH    51
#define DOUBLE_OFF_EXPONENT_LOW     52
#define DOUBLE_OFF_EXPONENT_HIGH    62
#define DOUBLE_OFF_SIGN             63

#define COLOR_PREFIX        "\033["
#define COLOR_SUFFIX        "m"
#define COLOR(COLOR_ID)     COLOR_PREFIX COLOR_ID COLOR_SUFFIX
#define COLOR_NONE          COLOR("0")
#define COLOR_BLUE          COLOR("34")
#define COLOR_GREEN         COLOR("32")
#define COLOR_RED           COLOR("31")

#define COLOR_SIGN          COLOR_BLUE
#define COLOR_EXPONENT      COLOR_GREEN
#define COLOR_FRACTION      COLOR_RED

#define TEXT_SEPERATOR "================================================================================================================================================================================================"
#define TEXT_31_TO_0   "31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0"
#define TEXT_63_TO_32  "63 62 61 60 59 58 57 56 55 54 53 52 51 50 49 48 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32"
#define TEXT_63_TO_0    TEXT_63_TO_32 " " TEXT_31_TO_0


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
            puts(
                "fpview by 7Ji, a bitwise viewer of IEEE754 floating number on terminal\n"
                "\n"
                "  fpview (-s/--single) (-f/--float) (-d/--double) (-b/--both) (--) [number 1] ([number 2] (...))\n"
                "\n"
                "  -h/--help\tprint this help message\n"
                "  -s/--single\tshow only single-precision bitwise view\n"
                "  -f/--float\talis to -s/--single\n"
                "  -d/--double\tshow only double-precision bitwise view\n"
                "  -b/--both\tshow both single-precision and double-precision bitwise views (default)\n"
                "\n"
                "Hint: due to the nature of argument parsing, you might want to add -- before all numbers\n"
                "\n"
                "E.g.:\n"
                "  To get bitwise views of single-precision fp numbers 1.0 and 2.0:\n"
                "    fpview --single 1.0 2.0\n"
                "  To get bitwise views of double-precision fp numbers -0.1 and -0.2:\n"
                "    fpview --double -- 0.1 0.2\n"
                "  To get bitwise views of both single-precision and double-precision fp numbers 1.234 -2.3441 5.61323 -10.2323:\n"
                "    fpview -- 1.234 -2.3441 5.61323 -10.2323"
            );
            return 0;
        case 'f':
        case 's':
            puts("> Specified -f/--float/-s/--single, would only show single-precision view");
            format = FloatingFormatSingle;
            break;
        case 'd':
            puts("> Specified -d/--double, would only show double-precision view");
            format = FloatingFormatDouble;
            break;
        case 'b':
            puts("> Specified -b/--both, would show both single-precision and double-precision view");
            format = FloatingFormatBoth;
            break;
        default:
            printf("Unknown option '%s', maybe try -h or --help", argv[optind - 1]);
            return 1;
        }
    }
    puts("Parts are colored:\n"COLOR_SIGN" sign"COLOR_EXPONENT" exponent"COLOR_FRACTION" fraction"COLOR_NONE);
    for (int i = optind; i < argc; ++i) {
        if (format & FloatingFormatSingle) {
            printf(TEXT_SEPERATOR"\n%s as single-precision: ", argv[i]);
            union {
                float number;
                uint32_t bytes;
            } overlap;
            char *end;
            overlap.number = strtof(argv[i], &end);
            if (overlap.number == 0.0 && end == argv[i]) {
                puts("BAD INPUT");
                continue;
            }
            char digits[32];
            for (short i = 0; i < 32; ++i) {
                digits[i] = (uint8_t)(overlap.bytes >> i) & 0b1;
            }
            char buffer[FLT_DIG + 3];
            bool exact = false;
            int len = sprintf(buffer, "%.*f", FLT_DIG, overlap.number);
            if (len > 0 && len <= FLT_DIG + 2) {
                float new = strtof(argv[i], &end);
                if (!(new == 0.0 && end == buffer)) {
                    exact = new == overlap.number;
                }
            }
            printf("%.*f (%s)/ %a / "COLOR_SIGN"%c"COLOR_NONE"1."COLOR_FRACTION, FLT_DIG, overlap.number, exact ? "exact" : "non-exact", overlap.number, digits[SINGLE_OFF_SIGN] ? '-' : '+');
            for (short i = SINGLE_OFF_FRACTION_HIGH; i >= SINGLE_OFF_FRACTION_LOW; --i) {
                printf("%hhu", digits[i]);
            }
            printf(COLOR_NONE"e("COLOR_EXPONENT);
            for (short i = SINGLE_OFF_EXPONENT_HIGH; i >= SINGLE_OFF_EXPONENT_LOW; --i) {
                printf("%hhu", digits[i]);
            }
            uint8_t e = (overlap.bytes >> (SINGLE_OFF_FRACTION_HIGH + 1)) & 0xff;
            printf(COLOR_NONE"-11111111)_2, "COLOR_EXPONENT"E=%hhu"COLOR_NONE", "COLOR_EXPONENT"exp=%hhd"COLOR_NONE"\n "TEXT_31_TO_0"\n"COLOR_SIGN"  %hhu"COLOR_EXPONENT"", e, (int8_t)(e - 127), digits[SINGLE_OFF_SIGN]);
            for (short i = SINGLE_OFF_EXPONENT_HIGH; i >= SINGLE_OFF_EXPONENT_LOW; --i) {
                printf("  %hhu", digits[i]);
            }
            printf(COLOR_FRACTION);
            for (short i = SINGLE_OFF_FRACTION_HIGH; i >= SINGLE_OFF_FRACTION_LOW; --i) {
                printf("  %hhu", digits[i]);
            }
            printf(COLOR_NONE"\n"COLOR_SIGN"sign"COLOR_NONE"       "COLOR_EXPONENT"exponent"COLOR_NONE"                                      "COLOR_FRACTION"fraction"COLOR_NONE"\n");
        }
        if (format & FloatingFormatDouble) {
            printf(TEXT_SEPERATOR"\n%s as double-precision: ", argv[i]);
            union {
                double number;
                uint64_t bytes;
            } overlap;
            char *end;
            overlap.number = strtod(argv[i], &end);
            if (overlap.number == 0.0 && end == argv[i]) {
                puts("BAD INPUT");
                continue;
            }
            char digits[64];
            for (short i = 0; i < 64; ++i) {
                digits[i] = (uint8_t)(overlap.bytes >> i) & 0b1;
            }
            char buffer[DBL_DIG + 3];
            bool exact = false;
            int len = sprintf(buffer, "%.*lf", DBL_DIG, overlap.number);
            if (len > 0 && len <= DBL_DIG + 2) {
                float new = strtod(argv[i], &end);
                if (!(new == 0.0 && end == buffer)) {
                    exact = new == overlap.number;
                }
            }
            printf("%.*lf (%s) / %a / "COLOR_SIGN"%c"COLOR_NONE"1."COLOR_FRACTION"", DBL_DIG, overlap.number, exact ? "exact" : "non-exact", overlap.number, digits[DOUBLE_OFF_SIGN] ? '-' : '+');
            for (short i = DOUBLE_OFF_FRACTION_HIGH; i >= DOUBLE_OFF_FRACTION_LOW; --i) {
                printf("%hhu", digits[i]);
            }
            printf(COLOR_NONE"e("COLOR_EXPONENT);
            for (short i = DOUBLE_OFF_EXPONENT_HIGH; i >= DOUBLE_OFF_EXPONENT_LOW; --i) {
                printf("%hhu", digits[i]);
            }
            uint16_t e = (overlap.bytes >> (DOUBLE_OFF_FRACTION_HIGH + 1)) & 0x7ff;
            printf(COLOR_NONE"-1111111111)_2, "COLOR_EXPONENT"E=%hu"COLOR_NONE", "COLOR_EXPONENT"exp=%hd"COLOR_NONE"\n "TEXT_63_TO_0"\n"COLOR_SIGN"  %hu"COLOR_EXPONENT"", e, (int16_t)(e - 1023), digits[DOUBLE_OFF_SIGN]);
            for (short i = DOUBLE_OFF_EXPONENT_HIGH; i >= DOUBLE_OFF_EXPONENT_LOW; --i) {
                printf("  %hhu", digits[i]);
            }
            printf(COLOR_FRACTION);
            for (short i = DOUBLE_OFF_FRACTION_HIGH; i >= DOUBLE_OFF_FRACTION_LOW; --i) {
                printf("  %hhu", digits[i]);
            }
            printf(COLOR_NONE"\n"COLOR_SIGN"sign"COLOR_NONE"            "COLOR_EXPONENT"exponent"COLOR_NONE"                                                                                      "COLOR_FRACTION"fraction"COLOR_NONE"\n");
        }
    }
    puts(TEXT_SEPERATOR);

    return 0;
}
