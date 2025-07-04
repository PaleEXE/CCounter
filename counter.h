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
    size_t doc_id, freq;
} TermFreq;

typedef struct {
    int *items;
    size_t capacity, count;
} ListInt;

typedef struct {
    Pair *items;
    size_t capacity, count;
} Counter;

typedef struct {
    Str term;
    TermFreq *items;
    size_t capacity, doc_freq;
} Posting;

typedef struct {
    ListStr collection;
    Posting *index;
    size_t capacity, count;
} InvertedIndex;

Counter counter_new();

void counter_resize(Counter *counter);

size_t hash(const Str *key, size_t mod);

bool insert(Counter *counter, Str key, size_t val);

size_t get(const Counter *counter, const Str *key);

size_t *get_mut(const Counter *counter, const Str *key);

void counter_print(const Counter *counter);

InvertedIndex inverted_index_new();

void resize_index(InvertedIndex *inverted_index);

bool is_here_term(const InvertedIndex *inverted_index, const Str *term, size_t *pos);

void add_document(InvertedIndex *inverted_index, char *file_path);

void index_print(const InvertedIndex *inverted_index);

#endif //COUNTER_H
