#include "common.h"
#include <errno.h>
#include <stdlib.h>

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;
    a = a % mod;
    while (b > 0) {
        if (b % 2 == 1)
            result = (result + a) % mod;
        a = (a * 2) % mod;
        b /= 2;
    }
    return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
    char *end = NULL;
    unsigned long long i = strtoull(str, &end, 10);
    if (errno == ERANGE) {
        return false;
    }

    if (errno != 0)
        return false;

    *val = i;
    return true;
}

uint64_t ComputeRangeProduct(uint64_t begin, uint64_t end, uint64_t mod) {
    uint64_t result = 1;
    for (uint64_t i = begin; i <= end; i++) {
        result = MultModulo(result, i, mod);
    }
    return result;
}