//
// Created by VICTUS on 7/2/2025.
//

#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Counter counter_new() {
    return (Counter){
        .items = (Pair *) calloc(CAPACITY, sizeof(Pair)),
        .capacity = CAPACITY,
        .count = 0
    };
}

void counter_resize(Counter *counter) {
    size_t new_capacity = counter->capacity * 2;
    Pair *newItems = (Pair *) calloc(new_capacity, sizeof(Pair));
    memcpy(newItems, counter->items, sizeof(Pair) * counter->capacity);
    free(counter->items);
    counter->items = newItems;
    counter->capacity = new_capacity;
}

size_t hash(const Str *key, size_t mod) {
    size_t rizz = 0;
    for (size_t i = 0; i < key->length; ++i) {
        rizz = (rizz << (i + 1)) * key->content[i] + 17;
        rizz %= mod;
    }
    return rizz;
}

bool is_here(const Counter *counter, const Str *key, size_t *pos) {
    size_t idx = hash(key, counter->capacity);

    while (counter->items[idx].key.content != NULL) {
        if (compare(&counter->items[idx].key, key)) {
            *pos = idx;
            return true; // already exists
        }
        idx = (idx + 1) % counter->capacity;
    }
    *pos = idx;
    return false;
}

bool insert(Counter *counter, Str key, size_t val) {
    if (counter->count == counter->capacity)
        counter_resize(counter);

    size_t pos;
    if (counter->items != NULL && is_here(counter, &key, &pos))
        return false;

    counter->items[pos] = (Pair){
        .key = key,
        .val = val
    };
    counter->count++;
    return true;
}

size_t get(const Counter *counter, const Str *key) {
    size_t pos;
    if (is_here(counter, key, &pos)) return counter->items[pos].val;
    return SIZE_MAX;
}

size_t *get_mut(const Counter *counter, const Str *key) {
    size_t pos;
    if (is_here(counter, key, &pos)) return &counter->items[pos].val;
    return nullptr;
}

void counter_print(const Counter *counter) {
    for (size_t i = 0; i < counter->capacity; ++i)
        if (counter->items[i].key.content != NULL) {
            str_print(&counter->items[i].key);
            printf(" \t=> %zu\n", counter->items[i].val);
        }
}
