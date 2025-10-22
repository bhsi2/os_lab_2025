#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>

// Структура для передачи аргументов вычислений
struct ComputeArgs {
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
};

// Функция для модульного умножения
uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

// Функция для преобразования строки в uint64_t
bool ConvertStringToUI64(const char *str, uint64_t *val);

// Функция для вычисления произведения в диапазоне по модулю
uint64_t ComputeRangeProduct(uint64_t begin, uint64_t end, uint64_t mod);

#endif