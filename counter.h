//
// Created by VICTUS on 7/2/2025.
//

#ifndef COUNTER_H
#define COUNTER_H

#include <stdio.h>
#include "str.h"

static FILE *s_file_out = nullptr;

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

typedef struct ListFloat {
    float *items;
    size_t capacity, count;
} ListFloat;

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

ListFloat list_float_new(size_t capacity);

void list_float_resize(ListFloat *list);

void list_float_append(ListFloat *list, float value);

void list_float_insert(ListFloat *list, size_t index, float value);

void list_float_remove(ListFloat *list, size_t index);

void list_float_print(const ListFloat *list);

ListFloat calc_tf_idf(const InvertedIndex *inverted_index, const char *query);

void scores_print(const InvertedIndex *inverted_index, const ListFloat *scores);


void set_output_file(FILE *file);

#endif //COUNTER_H
