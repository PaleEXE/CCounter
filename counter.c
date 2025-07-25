//
// Created by VICTUS on 7/2/2025.
//

#include "counter.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const size_t cap = 128;

Counter counter_new() {
    return (Counter){
        .items = (Pair *) calloc(CAPACITY, sizeof(Pair)),
        .capacity = CAPACITY,
        .count = 0
    };
}

void counter_resize(Counter *counter) {
    size_t new_capacity = counter->capacity * 2;
    Pair *new_items = calloc(new_capacity, sizeof(Pair));

    for (size_t i = 0; i < counter->capacity; ++i) {
        if (counter->items[i].key.content == nullptr) continue;

        Str key = counter->items[i].key;
        size_t val = counter->items[i].val;

        size_t idx = hash(&key, new_capacity);
        while (new_items[idx].key.content != nullptr) {
            idx = (idx + 1) % new_capacity;
        }

        new_items[idx] = (Pair){
            .key = key,
            .val = val
        };
    }

    free(counter->items);
    counter->items = new_items;
    counter->capacity = new_capacity;
}

size_t hash(const Str *key, size_t mod) {
    size_t hash = 5381;
    for (size_t i = 0; i < key->length; ++i) {
        hash = (hash << 5) + hash + key->content[i];
    }
    return hash % mod;
}

bool is_here(const Counter *counter, const Str *key, size_t *pos) {
    size_t idx = hash(key, counter->capacity);

    while (counter->items[idx].key.content != nullptr) {
        if (compare(&counter->items[idx].key, key)) {
            *pos = idx;
            return true; // already exists
        }
        idx = (idx + 1) % counter->capacity;
    }
    *pos = idx;
    return false;
}

bool insert(Counter *counter, const Str *key, size_t val) {
    if (counter->count == counter->capacity)
        counter_resize(counter);

    size_t pos;
    if (counter->items != nullptr && is_here(counter, key, &pos))
        return false;

    counter->items[pos] = (Pair){
        .key = str_clone(key),
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
        if (counter->items[i].key.content != nullptr) {
            str_print(&counter->items[i].key);
            printf(" \t=> %zu\n", counter->items[i].val);
        }
}

Counter counter_from_list(const ListStr *list) {
    Counter counter = counter_new();
    for (size_t i = 0; i < list->count; ++i) {
        if (!insert(&counter, &list->items[i], 1)) {
            *get_mut(&counter, &list->items[i]) += 1;
        }
    }
    return counter;
}

void counter_free(Counter *counter) {
    if (!counter) return;

    for (size_t i = 0; i < counter->capacity; i++) {
        if (counter->items[i].key.content) {
            free(counter->items[i].key.content);
        }
    }
    free(counter->items);
    counter->items = nullptr;
    counter->capacity = 0;
    counter->count = 0;
}

void counter_shallow_free(Counter *counter) {
    if (!counter) return;

    free(counter->items);
    counter->items = nullptr;
    counter->capacity = 0;
    counter->count = 0;
}

char *read_file(const char *path) {
    FILE *file = fopen(path, "r");

    if (file == nullptr) {
        fprintf(stderr, "ERROR: cannot open file: %s\n", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *text = (char *) calloc(size + 1, sizeof(char));
    fread(text, 1, size, file);
    text[size] = '\0';
    fclose(file);

    return text;
}

InvertedIndex inverted_index_new() {
    InvertedIndex inverted_index = (InvertedIndex){
        .collection = list_with_capacity(cap),
        .index = (Posting *) calloc(CAPACITY, sizeof(Posting)),
        .capacity = CAPACITY,
        .count = 0
    };
    return inverted_index;
}

void resize_index(InvertedIndex *inverted_index) {
    size_t new_capacity = inverted_index->capacity * 2;
    Posting *new_index = calloc(new_capacity, sizeof(Posting));

    for (size_t i = 0; i < inverted_index->capacity; ++i) {
        Posting old_post = inverted_index->index[i];
        if (old_post.term.content == nullptr) continue;

        size_t idx = hash(&old_post.term, new_capacity);
        while (new_index[idx].term.content != nullptr) {
            idx = (idx + 1) % new_capacity;
        }

        new_index[idx] = old_post;
    }

    free(inverted_index->index);
    inverted_index->index = new_index;
    inverted_index->capacity = new_capacity;
}

bool is_here_term(const InvertedIndex *inverted_index, const Str *term, size_t *pos) {
    size_t idx = hash(term, inverted_index->capacity);

    while (inverted_index->index[idx].term.content != nullptr) {
        if (compare(&inverted_index->index[idx].term, term)) {
            *pos = idx;
            return true;
        }

        idx = (idx + 1) % inverted_index->capacity;
    }

    *pos = idx;
    return false;
}

void resize_posting(Posting *posting) {
    const size_t new_capacity = posting->capacity * 2;
    TermFreq *new_items = (TermFreq *) calloc(new_capacity, sizeof(TermFreq));

    if (new_items != nullptr) {
        memcpy(new_items, posting->items, sizeof(TermFreq) * posting->doc_freq);
        free(posting->items);
        posting->items = new_items;
        posting->capacity = new_capacity;
    }
}

void append_posting(Posting *posting, const TermFreq term_freq) {
    if (posting->capacity == posting->doc_freq) {
        resize_posting(posting);
    }

    posting->items[posting->doc_freq++] = term_freq;
}

void append_index(InvertedIndex *inverted_index, const Str *term, const size_t doc_id, const size_t freq) {
    if (inverted_index->capacity == inverted_index->count) {
        resize_index(inverted_index);
    }
    if (inverted_index->index == nullptr) {
        return;
    }

    size_t pos;
    if (!is_here_term(inverted_index, term, &pos)) {
        Posting posting = (Posting){
            .term = str_clone(term),
            .items = (TermFreq *) calloc(cap, sizeof(TermFreq)),
            .capacity = cap,
            .doc_freq = 0
        };
        inverted_index->index[pos] = posting;
        inverted_index->count++;
    }
    TermFreq tf = (TermFreq){
        .doc_id = doc_id,
        .freq = freq
    };
    append_posting(&inverted_index->index[pos], tf);
}

void add_document(InvertedIndex *inverted_index, char *file_path) {
    char *content = read_file(file_path);
    if (content == nullptr) {
        free(content);
        return;
    }

    ListStr terms = split(content, false);
    Counter terms_counter = counter_from_list(&terms);
    append(&inverted_index->collection, to_str(file_path));
    const size_t doc_id = inverted_index->collection.count - 1;

    for (size_t i = 0; i < terms_counter.capacity; ++i) {
        if (terms_counter.items[i].key.content == nullptr) continue;
        append_index(inverted_index, &terms_counter.items[i].key, doc_id, terms_counter.items[i].val);
    }
    free(content);
    list_free(&terms);
    counter_free(&terms_counter);
}

Posting *get_posting(const InvertedIndex *inverted_index, const Str *term) {
    size_t pos;
    if (is_here_term(inverted_index, term, &pos)) {
        return &inverted_index->index[pos];
    }
    return nullptr;
}

void index_terms_print(const InvertedIndex *inverted_index) {
    for (size_t i = 0; i < inverted_index->capacity; ++i) {
        if (inverted_index->index[i].term.content == nullptr) continue;
        str_print_fix(&inverted_index->index[i].term, 20);
        printf("%zu\n", i);
    }
}

void index_print(const InvertedIndex *inverted_index) {
    if (!s_file_out) s_file_out = stdout;
    // Print header
    fprintf(s_file_out, "╔══════════════════════════════════════╗\n");
    fprintf(s_file_out, "║        INVERTED INDEX (%-3zu docs)     ║\n", inverted_index->collection.count);
    fprintf(s_file_out, "╚══════════════════════════════════════╝\n\n");

    // Print document collection
    fprintf(s_file_out, "Document Collection:\n");
    for (size_t i = 0; i < inverted_index->collection.count; ++i) {
        fprintf(s_file_out, "  [%zu] ", i);
        str_fprint_fix(&inverted_index->collection.items[i], 20, s_file_out);
        fprintf(s_file_out, "\n");
    }
    putc('\n', s_file_out);

    // Print terms with postings
    fprintf(s_file_out, "s_file_out, Terms (total %zu):\n", inverted_index->count);
    for (size_t i = 0; i < inverted_index->capacity; i++) {
        if (inverted_index->index[i].term.content != nullptr) {
            // Print term
            fprintf(s_file_out, "  • ");
            str_fprint_fix(&inverted_index->index[i].term, 20, s_file_out);
            fprintf(s_file_out, " (df=%zu): ", inverted_index->index[i].doc_freq);

            // Print postings
            for (size_t j = 0; j < inverted_index->index[i].doc_freq; j++) {
                fprintf(s_file_out,
                        "[%zu:%zu]",
                        inverted_index->index[i].items[j].doc_id,
                        inverted_index->index[i].items[j].freq);
                if (j < inverted_index->index[i].doc_freq - 1) {
                    fprintf(s_file_out, ", ");
                }
            }
            putc('\n', s_file_out);
        }
    }
}

ListFloat list_float_new(const size_t capacity) {
    ListFloat list = (ListFloat){
        .items = (float *) calloc(capacity, sizeof(float)),
        .capacity = capacity,
        .count = 0
    };
    return list;
}

void list_float_resize(ListFloat *list) {
    float *new_items = (float *) malloc(list->capacity * 2 * sizeof(float));
    memcpy(new_items, list->items, list->count * sizeof(float));
    free(list->items);
    list->items = new_items;
    list->capacity *= 2;
}

void list_float_append(ListFloat *list, const float value) {
    if (list->count == list->capacity) list_float_resize(list);
    list->items[list->count++] = value;
}

void list_float_insert(ListFloat *list, const size_t index, const float value) {
    if (list->count == list->capacity) list_float_resize(list);
    if (index > list->count) return;
    for (size_t i = list->count; i > index; --i)
        list->items[i] = list->items[i - 1];
    list->items[index] = value;
    list->count++;
}

void list_float_remove(ListFloat *list, size_t index) {
    if (index >= list->count) return;
    for (size_t i = index; i < list->count - 1; ++i)
        list->items[i] = list->items[i + 1];
    list->count--;
}

void list_float_print(const ListFloat *list) {
    for (size_t i = 0; i < list->count; ++i)
        printf("%f\n", list->items[i]);
}

ListFloat calc_tf_idf(const InvertedIndex *inverted_index, char *query) {
    ListFloat rizz = list_float_new(inverted_index->collection.count);
    rizz.count = rizz.capacity;

    ListStr terms = split(query, true);

    for (size_t i = 0; i < terms.count; ++i) {
        Str *term = &terms.items[i];
        Posting *term_posting = get_posting(inverted_index, term);
        if (term_posting == nullptr) {
            continue;
        }
        for (size_t j = 0; j < term_posting->doc_freq; ++j) {
            rizz.items[term_posting->items[j].doc_id] +=
                    log10f((float) term_posting->items[j].freq) *
                    log10f((float) inverted_index->collection.count /
                           (float) term_posting->doc_freq);
        }
    }

    return rizz;
}

void scores_print(const InvertedIndex *inverted_index, const ListFloat *scores) {
    typedef struct {
        size_t doc_id;
        float score;
    } DocScore;

    DocScore *entries = malloc(scores->count * sizeof(DocScore));
    for (size_t i = 0; i < scores->count; ++i) {
        entries[i].doc_id = i;
        entries[i].score = scores->items[i];
    }

    for (size_t i = 0; i < scores->count - 1; ++i) {
        for (size_t j = i + 1; j < scores->count; ++j) {
            if (entries[j].score > entries[i].score) {
                DocScore tmp = entries[i];
                entries[i] = entries[j];
                entries[j] = tmp;
            }
        }
    }

    printf("%-*s  %s\n", 36, "Document", "Score");
    printf("------------------------------------  --------\n");

    for (size_t i = 0; i < scores->count; ++i) {
        str_print_fix(&inverted_index->collection.items[entries[i].doc_id], 36);
        printf("  %f\n", entries[i].score);
    }

    free(entries);
}

void set_output_file(FILE *file) {
    s_file_out = file;
}
