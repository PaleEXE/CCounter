#include "str.h"
#include "libstemmer_c/include/libstemmer.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global stemmer instance (lazy initialized)
static struct sb_stemmer *stemmer = nullptr;

Str to_str(char *str) {
    if (!str) return (Str){.content = nullptr, .length = 0};
    return (Str){
        .content = str,
        .length = strlen(str)
    };
}

Str str_clone(const Str *s) {
    char *copy = malloc(s->length + 1);
    if (!copy) {
        fprintf(stderr, "Failed to clone string\n");
        exit(1);
    }
    memcpy(copy, s->content, s->length);
    copy[s->length] = '\0';
    return (Str){.content = copy, .length = s->length};
}

ListStr list_new(void) {
    Str *items = (Str *) malloc(sizeof(Str) * CAPACITY);
    if (!items) {
        fprintf(stderr, "Failed to allocate memory for ListStr\n");
        exit(EXIT_FAILURE);
    }
    return (ListStr){
        .items = items,
        .capacity = CAPACITY,
        .count = 0
    };
}

ListStr list_with_capacity(size_t capacity) {
    if (capacity == 0) capacity = 1; // Ensure at least 1 capacity
    Str *items = (Str *) malloc(sizeof(Str) * capacity);
    if (!items) {
        fprintf(stderr, "Failed to allocate memory for ListStr\n");
        exit(EXIT_FAILURE);
    }
    return (ListStr){
        .items = items,
        .capacity = capacity,
        .count = 0
    };
}

void str_print(const Str *str) {
    if (!str || !str->content) return;
    fwrite(str->content, 1, str->length, stdout);
}

void str_print_fix(const Str *str, size_t fix) {
    if (!str || !str->content) return;
    fwrite(str->content, 1, str->length, stdout);
    for (size_t i = str->length; i < fix; ++i)
        putchar(' ');
}

void str_fprint_fix(const Str *str, size_t fix, FILE *fout) {
    if (!str || !str->content || !fout) return;
    fwrite(str->content, 1, str->length, fout);
    for (size_t i = str->length; i < fix; ++i)
        putc(' ', fout);
}

bool compare(const Str *str1, const Str *str2) {
    if (!str1 || !str2) return false;
    if (str1->length != str2->length) return false;
    return memcmp(str1->content, str2->content, str1->length) == 0;
}

void resize(ListStr *list) {
    if (!list || list->capacity == SIZE_MAX / 2) {
        fprintf(stderr, "List capacity too large to resize\n");
        exit(EXIT_FAILURE);
    }

    size_t new_capacity = list->capacity * 2;
    Str *new_items = (Str *) realloc(list->items, sizeof(Str) * new_capacity);
    if (!new_items) {
        fprintf(stderr, "Failed to resize ListStr\n");
        exit(EXIT_FAILURE);
    }

    list->items = new_items;
    list->capacity = new_capacity;
}

void append(ListStr *list, Str str) {
    if (!list) return;

    if (list->count >= list->capacity) {
        resize(list);
    }

    list->items[list->count++] = str;
}

bool is_word_char(unsigned char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           c == '\''; // Allow apostrophes in words
}

void normalize(char *str) {
    if (!str) return;

    for (char *p = str; *p; p++) {
        unsigned char c = (unsigned char) *p;
        if (!is_word_char(c)) {
            *p = ' ';
        } else {
            *p = (char) tolower(c);
        }
    }
}

void list_print(const ListStr *list) {
    if (!list) return;
    for (size_t i = 0; i < list->count; ++i) {
        str_print(&list->items[i]);
        putchar('\n');
    }
}

void list_free(ListStr *list) {
    if (!list) return;

    // Free each string's content if it was allocated
    for (size_t i = 0; i < list->count; i++) {
        if (list->items[i].content) {
            free(list->items[i].content);
        }
    }
    free(list->items);
    list->items = nullptr;
    list->capacity = 0;
    list->count = 0;
}

void list_shallow_free(ListStr *list) {
    if (!list) return;

    free(list->items);
    list->items = nullptr;
    list->capacity = 0;
    list->count = 0;
}

void stemmer_create(void) {
    if (!stemmer) {
        stemmer = sb_stemmer_new("english", nullptr);
        if (!stemmer) {
            fprintf(stderr, "Failed to initialize stemmer\n");
            exit(EXIT_FAILURE);
        }
    }
}

void stemmer_free(void) {
    if (stemmer) {
        sb_stemmer_delete(stemmer);
        stemmer = nullptr;
    }
}

ListStr split(char *text, bool do_copy) {
    if (!text) return list_new();

    char *copy = text;
    if (do_copy) copy = strdup(text);
    if (!copy) {
        fprintf(stderr, "Faild to duplicate Text");
    }

    stemmer_create();
    ListStr result = list_new();

    normalize(copy);

    const char *delims = " \t\n\r-.,!?;:\"'()[]{}<>";
    char *token = strtok(copy, delims);

    while (token != NULL) {
        size_t token_len = strlen(token);
        if (token_len > 0) {
            const sb_symbol *stemmed = sb_stemmer_stem(
                stemmer,
                (const sb_symbol *) token,
                (int) token_len
            );
            if (!stemmed) {
                fprintf(stderr, "Stemming failed for token: %s\n", token);
                list_free(&result);
                free(copy);
                return list_new();
            }

            size_t stemmed_len = sb_stemmer_length(stemmer);
            char *stemmed_copy = malloc(stemmed_len + 1);
            if (!stemmed_copy) {
                fprintf(stderr, "Failed to allocate memory for stemmed token\n");
                list_free(&result);
                free(copy);
                exit(EXIT_FAILURE);
            }

            memcpy(stemmed_copy, stemmed, stemmed_len);
            stemmed_copy[stemmed_len] = '\0';

            append(&result, (Str){
                       .content = stemmed_copy,
                       .length = stemmed_len
                   });
        }

        token = strtok(nullptr, delims);
    }
    if (do_copy) free(copy);

    return result;
}
