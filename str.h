//
// Created by VICTUS on 7/2/2025.
//

#ifndef STR_H
#define STR_H

#include <stdio.h>
#define CAPACITY 1024

typedef struct {
    char *content;
    size_t length;
} Str;

typedef struct {
    Str *items;
    size_t capacity, count;
} ListStr;

Str to_str(char *str);

Str str_clone(const Str *s);

ListStr list_new();

ListStr list_with_capacity(size_t capacity);

void str_print(const Str *str);

void str_print_fix(const Str *str, size_t fix);

void str_fprint_fix(const Str *str, size_t fix, FILE *fout);

bool compare(const Str *str1, const Str *str2);

void resize(ListStr *list);

void append(ListStr *list, Str str);

ListStr split(char *text, bool do_copy);

void list_print(const ListStr *list);

void list_free(ListStr *list);

void list_shallow_free(ListStr *list);

#endif //STR_H
