//
// Created by VICTUS on 7/2/2025.
//

#include "str.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


const Str to_str(char *str) {
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

void str_print(const Str *str) {
    fwrite(str->content, 1, str->length + 1, stdout);
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
    Str *newItems = malloc(sizeof(Str) * new_capacity);

    if (list->items != NULL) {
        memcpy(newItems, list->items, sizeof(Str) * list->count);
        free(list->items);
    }

    list->items = newItems;
    list->capacity = new_capacity;
}

void append(ListStr *list, Str str) {
    if (list->capacity == list->count) {
        resize(list);
    }

    list->items[list->count++] = str;
}

ListStr split(char *text) {
    ListStr rizz = list_new();
    char *start = text;
    size_t len = 0;

    while (*text) {
        if (*text != ' ') {
            len++;
            text++;
            continue;
        }

        if (len > 0) {
            Str word = (Str){
                .content = start,
                .length = len,
            };
            append(&rizz, word);
        }

        text++;
        start = text;
        len = 0;
    }

    if (len > 0) {
        Str word = (Str){
            .content = start,
            .length = len,
        };
        append(&rizz, word);
    }

    return rizz;
}
