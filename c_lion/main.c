#include "str.h"
#include "counter.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const ListStr words = split("Mohammad Fadi Al Hennawi Mohammad Al Al al");

    Counter words_counter = counter_new();

    for (size_t i = 0; i < words.count; ++i) {
        Str *word = &words.items[i];

        if (!insert(&words_counter, *word, 1))
            *get_mut(&words_counter, word) += 1;
    }
    counter_print(&words_counter);

    return 0;
}
