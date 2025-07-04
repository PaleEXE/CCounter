#include "counter.h"
#include <stdlib.h>

#define FILES_COUNT 4

char *files[FILES_COUNT] = {
    "../songs/LoveDrug.txt",
    "../songs/Timeless.txt",
    "../songs/Billie Jean.txt",
    "../songs/Epitaph.txt"
};

int main(void) {
    InvertedIndex inverted_index = inverted_index_new();

    for (size_t i = 0; i < FILES_COUNT; ++i) {
        add_document(&inverted_index, files[i]);
    }
    index_print(&inverted_index);
    return 0;
}
