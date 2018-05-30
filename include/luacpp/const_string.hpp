#pragma once
#if __clang__
#if __has_extension(cxx_string_literal_templates)
asdsa

#define CONST_STRING(...) __VA_ARGS__
#endif
#endif

#ifndef CONST_STRING
#define CONST_STRING(s)                                                     \
    GET_CHAR(s, 0), GET_CHAR(s, 1), GET_CHAR(s, 2), GET_CHAR(s, 3),         \
	GET_CHAR(s, 4), GET_CHAR(s, 5), GET_CHAR(s, 6), GET_CHAR(s, 7),     \
	GET_CHAR(s, 8), GET_CHAR(s, 9), GET_CHAR(s, 10), GET_CHAR(s, 11),   \
	GET_CHAR(s, 12), GET_CHAR(s, 13), GET_CHAR(s, 14), GET_CHAR(s, 15), \
	GET_CHAR(s, 16), GET_CHAR(s, 17), GET_CHAR(s, 18), GET_CHAR(s, 19), \
	GET_CHAR(s, 20), GET_CHAR(s, 21), GET_CHAR(s, 22), GET_CHAR(s, 23), \
	GET_CHAR(s, 24), GET_CHAR(s, 25), GET_CHAR(s, 26), GET_CHAR(s, 27), \
	GET_CHAR(s, 28), GET_CHAR(s, 29), GET_CHAR(s, 30), GET_CHAR(s, 31), \
	GET_CHAR(s, 32), GET_CHAR(s, 33), GET_CHAR(s, 34), GET_CHAR(s, 35), \
	GET_CHAR(s, 36), GET_CHAR(s, 37), GET_CHAR(s, 38), GET_CHAR(s, 39), \
	GET_CHAR(s, 40), GET_CHAR(s, 41), GET_CHAR(s, 42), GET_CHAR(s, 43), \
	GET_CHAR(s, 44), GET_CHAR(s, 45), GET_CHAR(s, 46), GET_CHAR(s, 47), \
	GET_CHAR(s, 48), GET_CHAR(s, 49), GET_CHAR(s, 50), GET_CHAR(s, 51), \
	GET_CHAR(s, 52), GET_CHAR(s, 53), GET_CHAR(s, 54), GET_CHAR(s, 55), \
	GET_CHAR(s, 56), GET_CHAR(s, 57), GET_CHAR(s, 58), GET_CHAR(s, 59), \
	GET_CHAR(s, 60), GET_CHAR(s, 61), GET_CHAR(s, 62), GET_CHAR(s, 63), \
	GET_CHAR(s, 64), GET_CHAR(s, 65), GET_CHAR(s, 66), GET_CHAR(s, 67), \
	GET_CHAR(s, 68), GET_CHAR(s, 69), GET_CHAR(s, 70), GET_CHAR(s, 71), \
	GET_CHAR(s, 72), GET_CHAR(s, 73), GET_CHAR(s, 74), GET_CHAR(s, 75), \
	GET_CHAR(s, 76), GET_CHAR(s, 77), GET_CHAR(s, 78), GET_CHAR(s, 79), \
	GET_CHAR(s, 80), GET_CHAR(s, 81), GET_CHAR(s, 82), GET_CHAR(s, 83), \
	GET_CHAR(s, 84), GET_CHAR(s, 85), GET_CHAR(s, 86), GET_CHAR(s, 87), \
	GET_CHAR(s, 88), GET_CHAR(s, 89), GET_CHAR(s, 90), GET_CHAR(s, 91), \
	GET_CHAR(s, 92), GET_CHAR(s, 93), GET_CHAR(s, 94), GET_CHAR(s, 95), \
	GET_CHAR(s, 96), GET_CHAR(s, 97), GET_CHAR(s, 98), GET_CHAR(s, 99), \
	GET_CHAR(s, 100)

#define CONST_STRING_MIN(x, y) (x) < (y) ? (x) : (y)

#define GET_CHAR(string, i)                                                   \
    CONST_STRING_MIN(100, i) < (sizeof(string) / sizeof(*string)) ? string[i] \
								  : 0
#endif // CONST_STRING
