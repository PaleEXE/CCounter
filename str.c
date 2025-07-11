//
// Created by VICTUS on 7/2/2025.
//

#include "str.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


Str to_str(char *str) {
    return (Str){
        .content = str,
        .length = strlen(str)
    };
}

ListStr list_new() {
    return (ListStr){
        .items = (Str *) malloc(sizeof(Str) * CAPACITY),
        .capacity = CAPACITY,
        .count = 0
    };
}

ListStr list_with_capacity(size_t capacity) {
    return (ListStr){
        .items = (Str *) malloc(sizeof(Str) * capacity),
        .capacity = capacity,
        .count = 0
    };
}

void str_print(const Str *str) {
    fwrite(str->content, 1, str->length, stdout);
}

void str_print_fix(const Str *str, size_t fix) {
    fwrite(str->content, 1, str->length, stdout);
    for (size_t i = str->length; i < fix; ++i)
        putchar(' ');
}

void str_fprint_fix(const Str *str, size_t fix, FILE *fout) {
    fwrite(str->content, 1, str->length, fout);
    for (size_t i = str->length; i < fix; ++i)
        putc(' ', fout);
}

bool compare(const Str *str1, const Str *str2) {
    if (str1->length != str2->length)
        return false;

    for (size_t i = 0; i < str1->length; ++i) {
        if (str1->content[i] != str2->content[i]) {
            return false;
        }
    }
    return true;
}

void resize(ListStr *list) {
    size_t new_capacity = list->capacity * 2;
    Str *new_items = malloc(sizeof(Str) * new_capacity);

    if (list->items != NULL) {
        memcpy(new_items, list->items, sizeof(Str) * list->count);
        free(list->items);
    }

    list->items = new_items;
    list->capacity = new_capacity;
}

void append(ListStr *list, Str str) {
    if (list->capacity == list->count) {
        resize(list);
    }

    list->items[list->count++] = str;
}

bool is_word_char(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
}

void normalize(char *str) {
    if (!str) return;

    while (*str) {
        unsigned char c = (unsigned char) *str;

        if (c < 32 || c > 126) {
            *str = ' ';
        } else {
            *str = (char) tolower(c);
        }

        str++;
    }
}

void list_print(const ListStr *list) {
    for (size_t i = 0; i < list->count; ++i) {
        str_print(&list->items[i]);
        putchar('\n');
    }
}

ListStr split(char *text) {
    ListStr rizz = list_new();

    size_t len = strlen(text);
    char *copy = malloc(len + 1); // +1 for null terminator
    if (!copy) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }
    strcpy(copy, text);

    normalize(copy);

    const char *delims = " \t\n\r-.,!?;:\"'()[]{}<>";
    char *token = strtok(copy, delims);

    while (token != NULL) {
        size_t token_len = strlen(token);
        Str word = {
            .content = token,
            .length = token_len
        };
        append(&rizz, word);
        token = strtok(nullptr, delims);
    }

    return rizz;
}
