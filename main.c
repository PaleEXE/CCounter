#include "counter.h"
#include "file_util.h"
#include <stdlib.h>
#include <stdio.h>


int main(void) {
    size_t count;
    InvertedIndex inverted_index = inverted_index_new();

    char **files = get_files_in_folder("..\\songs", &count);

    for (size_t i = 0; i < count; ++i) {
        add_document(&inverted_index, files[i]);
    }

    FILE *outf = fopen("../inverted_index.txt", "w");
    set_output_file(outf);
    index_print(&inverted_index);

    ListFloat rizz = calc_tf_idf(&inverted_index, "just");
    scores_print(&inverted_index, &rizz);

    rizz = calc_tf_idf(&inverted_index, "JUST");

    scores_print(&inverted_index, &rizz);

    index_dump_json(&inverted_index, "../index.json");
    if (files) free_files(files, count);
    free(rizz.items);
    return 0;
}
