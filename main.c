#include "str.h"
#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *read_file(const char *path) {
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        fprintf(stderr, "ERROR: can not open file: %s\n", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *text = (char *) malloc(size + 1);
    fread(text, 1, size, file);
    text[size] = '\0';
    fclose(file);

    return text;
}

int main(void) {
    char *content = read_file("../LoveDrug.txt");
    const ListStr words = split(content);

    Counter words_counter = counter_new();

    for (size_t i = 0; i < words.count; ++i) {
        Str *word = &words.items[i];

        if (!insert(&words_counter, *word, 1))
            *get_mut(&words_counter, word) += 1;
    }
    counter_print(&words_counter);

    free(words.items);
    free(words_counter.items);
    free(content);

    return 0;
}
