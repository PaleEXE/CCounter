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

    ListFloat rizz = calc_tf_idf(&inverted_index, "she is a killer queen");
    scores_print(&inverted_index, &rizz);

    /*index_dump_json(&inverted_index, "../index.json");*/

    // load and dump do not work simultaneously!!!!!
    InvertedIndex inverted_index_loaded = inverted_index_new();
    if (!index_load_json(&inverted_index_loaded, "../index.json")) return 1;

    rizz = calc_tf_idf(&inverted_index_loaded, "she is a killer queen");
    scores_print(&inverted_index_loaded, &rizz);

    if (files) free_files(files, count);
    free(rizz.items);
    return 0;
}
