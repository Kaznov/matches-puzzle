#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

// Seven segment display
typedef enum DigitDisplay{
    kNoneSeven = 0,
    kTopMid    = 1 << 0,
    kTopLeft   = 1 << 1,
    kTopRight  = 1 << 2,
    kMidMid    = 1 << 3,
    kBotLeft   = 1 << 4,
    kBotMid    = 1 << 5,
    kBotRight  = 1 << 6
} DigitDisplay;

#define DIGITS_COUNT 10
const enum DigitDisplay kDigits[DIGITS_COUNT] = {
    kTopMid | kTopLeft | kTopRight | kBotLeft | kBotRight | kBotMid, // 0
    kTopRight | kBotRight,                                           // 1
    kTopMid | kTopRight | kMidMid | kBotLeft | kBotMid,              // 2
    kTopMid | kTopRight | kMidMid | kBotRight | kBotMid,             // 3
    kTopLeft | kTopRight | kMidMid | kBotRight,                      // 4
    kTopMid | kTopLeft | kMidMid | kBotRight | kBotMid,              // 5
    kTopMid | kTopLeft | kMidMid | kBotLeft | kBotMid | kBotRight,   // 6
    kTopMid | kTopRight | kBotRight,                                 // 7
    kTopMid | kTopLeft | kTopRight | kMidMid | kBotLeft
            | kBotMid | kBotRight,                                   // 8
    kTopMid | kTopLeft | kTopRight | kMidMid | kBotMid | kBotRight,  // 9  
};

const enum DigitDisplay kNumberEleven = kTopLeft | kTopRight
                                                 | kBotLeft | kBotRight;

const enum DigitDisplay kErrorDigit = kTopMid | kTopLeft | kMidMid
                                              | kBotLeft | kBotMid;

typedef enum SymbolDisplay {
    kNoneSymbol  = 0,
    kHorizontal1 = 1 << 0,
    kHorizontal2 = 1 << 1,
    kDiagonal1   = 1 << 2,
    kDiagonal2   = 1 << 3,
    kVertical    = 1 << 4,
} SymbolDisplay;

typedef enum SymbolName {
    kSymbolPlus,
    kSymbolMinus,
    kSymbolMultiply,
    kSymbolDivide,
    kSymbolEqual,
    kSymbolEmpty,
    kSymbolInvalid
} SymbolName;

#define SYMBOLS_COUNT 7
const enum SymbolDisplay kSymbols[SYMBOLS_COUNT] = {
    /*[kSymbolPlus]     =*/ kHorizontal1 | kVertical,
    /*[kSymbolMinus]    =*/ kHorizontal1,
    /*[kSymbolMultiply] =*/ kDiagonal1 | kDiagonal2,
    /*[kSymbolDivide]   =*/ kDiagonal1,
    /*[kSymbolEqual]    =*/ kHorizontal1 | kHorizontal2,
    /*[kSymbolEmpty]    =*/ kNoneSymbol,
    /*[kSymbolInvalid]  =*/ kHorizontal1 | kHorizontal2
                                         | kDiagonal1 | kDiagonal2
                                         | kVertical
};

const int8_t kInvalidDigit = -1;
const int8_t kEmptyDigit = -2;

#define DIGIT_MAP_SIZE 128
#define SYMBOL_MAP_SIZE 32

int8_t digit_display_to_value[DIGIT_MAP_SIZE];
int8_t symbol_display_to_value[SYMBOL_MAP_SIZE];

void initializeDigitMap(int8_t digit_map[]) {
    for (int i = 0; i < DIGIT_MAP_SIZE; ++i) {
        digit_map[i] = kInvalidDigit;
    }

    // empty display
    digit_map[kNoneSeven] = kEmptyDigit;

    // all digits representations
    for (int digit = 0; digit < DIGITS_COUNT; ++digit) {
        digit_map[kDigits[digit]] = digit;
    }

    // special case, 11
    digit_map[kNumberEleven] = 11;
}

void initializeSymbolMap(int8_t symbol_map[]) {
    for (int i = 0; i < SYMBOL_MAP_SIZE; ++i) {
        symbol_map[i] = kSymbolInvalid;
    }

    symbol_map[kNoneSymbol] = kSymbolEmpty;
    for (int symbol = 0; symbol < SYMBOLS_COUNT; ++symbol) {
        symbol_map[kSymbols[symbol]] = symbol;
    }
}

void printDigit(int digit) {
    int8_t digit_display = kDigits[digit];
    const char top_mid_fill   = digit_display & kTopMid   ? '#' : ' ';
    const char top_left_fill  = digit_display & kTopLeft  ? '#' : ' ';
    const char top_right_fill = digit_display & kTopRight ? '#' : ' ';
    const char mid_mid_fill   = digit_display & kMidMid   ? '#' : ' ';
    const char bot_left_fill  = digit_display & kBotLeft  ? '#' : ' ';
    const char bot_right_fill = digit_display & kBotRight ? '#' : ' ';
    const char bot_mid_fill   = digit_display & kBotMid   ? '#' : ' ';

    puts(top_mid_fill == '#' ? "######" : "      ");

    for (int i = 0; i < 4; ++i) {
        printf("%c    %c\n", top_left_fill, top_right_fill);
    }

    puts(mid_mid_fill == '#' ? "######" : "      ");

    for (int i = 0; i < 4; ++i) {
        printf("%c    %c\n", bot_left_fill, bot_right_fill);
    }
    
    puts(bot_mid_fill == '#' ? "######" : "      ");
}

void addSymbolSegments(char symbol_screen[5][6], const char new_segment[5][6]) {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (new_segment[i][j] == '#') {
                symbol_screen[i][j] = '#';
            }
        }
    }
}

void printSymbol(SymbolName symbol) {
    int8_t symbol_display = kSymbols[symbol];
    char symbol_screen[5][6] = {
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
    };
    const char dash1[5][6] = {
        "     ",
        "     ",
        "#####",
        "     ",
        "     "
    };
    const char dash2[5][6] = {
        "     ",
        "     ",
        "     ",
        "     ",
        "#####"
    };
    const char diagonal1[5][6] = {
        "    #",
        "   # ",
        "  #  ",
        " #   ",
        "#    "
    };
    const char diagonal2[5][6] = {
        "#    ",
        " #   ",
        "  #  ",
        "   # ",
        "    #"
    };
    const char vertical[5][6] = {
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  "
    };
    
    if (symbol_display & kHorizontal1) {
        addSymbolSegments(symbol_screen, dash1);
    }
    if (symbol_display & kHorizontal2) {
        addSymbolSegments(symbol_screen, dash2);
    }
    if (symbol_display & kDiagonal1) {
        addSymbolSegments(symbol_screen, diagonal1);
    }
    if (symbol_display & kDiagonal2) {
        addSymbolSegments(symbol_screen, diagonal2);
    }
    if (symbol_display & kVertical) {
        addSymbolSegments(symbol_screen, vertical);
    }
    
    for (int i = 0; i < 5; ++i) {
        puts(symbol_screen[i]);
    }
}

typedef struct Puzzle {
    enum SymbolName symbols[8];
    int digits[8];
    int size;
} Puzzle;

typedef struct PuzzleDisplay {
    enum SymbolDisplay symbols[8];
    enum DigitDisplay digits[8];
    int size;
} PuzzleDisplay;

int isValidStartPuzzle(const Puzzle* puzzle) {
    if (puzzle->symbols[0] != kSymbolEmpty) {
        return 0;
    }

    for (int i = 1; i < puzzle->size; ++i) {
        if (puzzle->symbols[i] == kSymbolInvalid ||
            puzzle->symbols[i] == kSymbolEmpty) {
            return 0;
        }
    }

    for (int i = 0; i < puzzle->size; ++i) {
        if (puzzle->digits[i] == kInvalidDigit ||
            puzzle->digits[i] == kEmptyDigit ||
            puzzle->digits[i] == 11) {
            return 0;
        }
    }

    // count equal signs, must be exactly one
    int equal_sign_count = 0;
    for (int i = 1; i < puzzle->size; ++i) {
        equal_sign_count += (puzzle->symbols[i] == kSymbolEqual);
    }
    if (equal_sign_count != 1) {
        return 0;
    }

    return 1;
}

#define MATH_ERROR INT_MIN

int evaluatePuzzleSide(const Puzzle* puzzle, int start, int length) {
    assert(length > 0 && length <= 8);
    assert(start + length <= puzzle->size);

    int numbers[8];
    enum SymbolName symbols[8];
    for (int i = 0; i < length; ++i) {
        numbers[i] = puzzle->digits[start + i];
        symbols[i] = puzzle->symbols[start + i];
    }

    // first, apply unary symbol from begining, if present
    if (symbols[0] == kSymbolMinus) {
        numbers[0] = -numbers[0];
    }

    // then, apply empty symbols (concatenation)
    for (int i = 1; i < length; ++i) {
        if (symbols[i] != kSymbolEmpty) {
            continue;
        }
        numbers[i - 1] *= ((numbers[i] >= 10) ? 100 : 10);
        numbers[i - 1] += numbers[i];

        for (int j = i; j < length - 1; ++j) {
            numbers[j] = numbers[j+1];
            symbols[j] = symbols[j+1];
        }

        --length;
        --i;
    }


    // then, apply multiplications and divisions
    for (int i = 1; i < length; ++i) {
        if (symbols[i] != kSymbolMultiply &&
            symbols[i] != kSymbolDivide) {
            continue;
        }

        if (symbols[i] == kSymbolMultiply) {
            numbers[i - 1] *= numbers[i];
        }

        if (symbols[i] == kSymbolDivide) {
            if (numbers[i] == 0) {
                return MATH_ERROR;
            }
            if (numbers[i - 1] % numbers[i] != 0) {
                return MATH_ERROR;
            }
            numbers[i - 1] /= numbers[i];
        }

        for (int j = i; j < length - 1; ++j) {
            numbers[j] = numbers[j+1];
            symbols[j] = symbols[j+1];
        }

        --length;
        --i;
    }

    // then, apply addition and subtraction
    
    // then, apply multiplications and divisions
    for (int i = 1; i < length; ++i) {
        if (symbols[i] != kSymbolPlus &&
            symbols[i] != kSymbolMinus) {
            continue;
        }

        if (symbols[i] == kSymbolPlus) {
            numbers[i - 1] += numbers[i];
        }

        if (symbols[i] == kSymbolMinus) {
            numbers[i - 1] -= numbers[i];
        }

        for (int j = i; j < length - 1; ++j) {
            numbers[j] = numbers[j+1];
            symbols[j] = symbols[j+1];
        }

        --length;
        --i;
    }

    assert(length == 1);
    return numbers[0];
}

int isValidEquation(const Puzzle* puzzle) {
    // First, find the position of the equalSign
    int eq_sign_position = -1;
    for (int i = 0; i < puzzle->size; ++i) {
        if (puzzle->symbols[i] == kSymbolEqual) {
            eq_sign_position = i;
            break;
        }
    }

    // evaluate both sides
    int left = evaluatePuzzleSide(puzzle, 0, eq_sign_position);
    int right = evaluatePuzzleSide(puzzle, eq_sign_position,
                                   puzzle->size - eq_sign_position);
    return left == right && left != MATH_ERROR;
}

int isValidEndPuzzle(const Puzzle* puzzle) {
    if (puzzle->symbols[0] == kSymbolInvalid ||
        puzzle->symbols[0] == kSymbolEqual ||
        puzzle->symbols[0] == kSymbolMultiply ||
        puzzle->symbols[0] == kSymbolDivide) {
        return 0;
    }

    for (int i = 1; i < puzzle->size; ++i) {
        if (puzzle->symbols[i] == kSymbolInvalid) {
            return 0;
        }
    }
    
    for (int i = 0; i < puzzle->size; ++i) {
        if (puzzle->digits[i] == kInvalidDigit) {
            return 0;
        }
    }

    // count equal signs, must be exactly one
    int equal_sign_count = 0;
    for (int i = 1; i < puzzle->size; ++i) {
        equal_sign_count += (puzzle->symbols[i] == kSymbolEqual);
    }
    if (equal_sign_count != 1) {
        return 0;
    }

    return isValidEquation(puzzle);
}

void puzzleDisplayToPuzzle(const PuzzleDisplay* display,
                           Puzzle* puzzle) {
    puzzle->size = display->size;
    for (int i = 0; i < puzzle->size; ++i) {
        puzzle->symbols[i] = symbol_display_to_value[display->symbols[i]];
        puzzle->digits[i] = digit_display_to_value[display->digits[i]]; 
    }
}

void puzzleToDisplay(const Puzzle* puzzle,
                     PuzzleDisplay* display) {
    display->size = puzzle->size;
    for (int i = 0; i < puzzle->size; ++i) {
        display->symbols[i] = kSymbols[puzzle->symbols[i]];    
        display->digits[i] = (puzzle->digits[i] == kInvalidDigit) ? kErrorDigit
                           : (puzzle->digits[i] == kEmptyDigit) ? kNoneSeven
                           : kDigits[puzzle->digits[i]];
    
    }
}

void printPuzzleAsText(const Puzzle* puzzle)
{
    for (int i = 0; i < puzzle->size; ++i) {
        switch (puzzle->symbols[i])
        {
        case kSymbolPlus:
            putchar('+');
            break;
        
        case kSymbolMinus:
            putchar('-');
            break;
        
        case kSymbolMultiply:
            putchar('*');
            break;
        
        case kSymbolDivide:
            putchar('/');
            break;
        
        case kSymbolEqual:
            putchar('=');
            break;

        case kSymbolInvalid:
            putchar('?');
            break;
        
        case kSymbolEmpty:
            break;
        }
        
        if (puzzle->digits[i] >= 0) {
            printf("%d", puzzle->digits[i]);
        } else if (puzzle->digits[i] == kEmptyDigit) {
        } else { // error digit
            putchar('#');
        }
    }
    putchar('\n');
}

void scanPuzzleFromText(Puzzle* puzzle) {
    // TODO
}

int getIthBit(const PuzzleDisplay* puzzle, int bit_id) {
    int i = bit_id / (7 + 5);
    int bit = bit_id % (7 + 5);
    assert(i < puzzle->size);

    if (bit < 7) return (puzzle->digits[i] >> bit) & 1;
    else         return (puzzle->symbols[i] >> (bit - 7)) & 1;
}


void flipIthBit(PuzzleDisplay* puzzle, int bit_id) {
    int i = bit_id / (7 + 5);
    int bit = bit_id % (7 + 5);
    assert(i < puzzle->size);

    if (bit < 7) puzzle->digits[i] ^= (1 << bit);
    
    else         puzzle->symbols[i] ^= (1 << (bit - 7));
}

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

int power(int v, int exp) {
    int result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= v;
    }
    return result;
}

int main(int argc, char* args[]) {
    int puzzle_size;
    if (argc == 1) {
        puzzle_size = 3;
    } else if (argc == 2) {
        if ((puzzle_size = atoi(args[1])) <= 2) {
            fputs("Invalid number of digits in the puzzle\n",
                  stderr);
            return EXIT_FAILURE;
        }
    } else {
        fputs("Program takes one argument, number of digits for the puzzle\n",
              stderr);
    }

    printf("Move a match puzzle! \n"
           "You can move a single match into a new position\n\n");

    initializeDigitMap(digit_display_to_value);
    initializeSymbolMap(symbol_display_to_value);

    pcg32_random_t rng = {time(NULL), 3};
    
    // Let's try and find a random puzzle solution, then change it to get
    // a puzzle
    int digit_v_limit = power(11, puzzle_size);
    int symbol_v_limit = power(6, puzzle_size);

    Puzzle solution;
    Puzzle puzzle;
    solution.size = puzzle.size = puzzle_size;
        
    while (1) {
        int digit_v = pcg32_random_r(&rng) % digit_v_limit;
        int symbol_v = pcg32_random_r(&rng) % symbol_v_limit;
        for (int i = 0; i < puzzle_size; ++i) {
            solution.digits[i] = digit_v % 11;
            digit_v /= 11;
            solution.digits[i] += solution.digits[i] == 10;
            solution.symbols[i] = symbol_v % 6;
            symbol_v /= 6;
        }

        if (!isValidEndPuzzle(&solution)) {
            continue;
        }

        //printPuzzleAsText(&solution);
        
        PuzzleDisplay solution_display;
        puzzleToDisplay(&solution, &solution_display);
        PuzzleDisplay puzzle_display = solution_display;
        // now, let's try and remove, then add a match

        for (int remove_position = 0;
             remove_position < puzzle_size * (7 + 5);
             ++remove_position) {
            if (getIthBit(&puzzle_display, remove_position) != 1) {
                continue;
            }
            flipIthBit(&puzzle_display, remove_position);
            
            for (int add_position = 0;
                 add_position < puzzle_size * (7 + 5);
                 ++add_position) {
                if (add_position == remove_position) { 
                    continue;
                }

                if (getIthBit(&solution_display, add_position) != 0) {
                    continue;
                }
                flipIthBit(&puzzle_display, add_position);
                puzzleDisplayToPuzzle(&puzzle_display, &puzzle);
                if (isValidStartPuzzle(&puzzle)) {
                    printPuzzleAsText(&puzzle);
                    return 0;
                }

                flipIthBit(&puzzle_display, add_position);
            }

            
            flipIthBit(&puzzle_display, remove_position);
        }

    }
}
