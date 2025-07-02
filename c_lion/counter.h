//
// Created by VICTUS on 7/2/2025.
//

#ifndef COUNTER_H
#define COUNTER_H

#include <stddef.h>
#include "str.h"

typedef struct {
    Str key;
    size_t val;
} Pair;

typedef struct {
    Pair *items;
    size_t capacity, count;
} Counter;

Counter counter_new();

void counter_resize(Counter *counter);

size_t hash(const Str *key, size_t mod);

bool insert(Counter *counter, Str key, size_t val);

size_t get(const Counter *counter, const Str *key);

size_t *get_mut(const Counter *counter, const Str *key);

void counter_print(const Counter *counter);

#endif //COUNTER_H
