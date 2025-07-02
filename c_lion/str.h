//
// Created by VICTUS on 7/2/2025.
//

#ifndef STR_H
#define STR_H

#include <stddef.h>
#define CAPACITY 1024

typedef struct {
    char *content;
    size_t length;
} Str;

typedef struct {
    Str *items;
    size_t capacity, count;
} ListStr;

const Str to_str(char *str);
ListStr list_new();
void str_print(const Str *str);
bool compare(const Str *str1, const Str *str2);
void resize(ListStr *list);
void append(ListStr *list, Str str);
ListStr split(char *text);

#endif //STR_H
