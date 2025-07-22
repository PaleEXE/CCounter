//
// Created by VICTUS on 7/22/2025.
//

#include "file_util.h"
#define JIM_IMPLEMENTATION
#include "jim/jim.h"
#include "counter.h"
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_PATH_ 512


char **get_files_in_folder(const char *folder, size_t *out_count) {
    char search_path[MAX_PATH_];
    // Manual string copy instead of snprintf
    size_t len = 0;
    while (folder[len] != '\0' && len < MAX_PATH_ - 3) {
        search_path[len] = folder[len];
        len++;
    }
    search_path[len++] = '\\';
    search_path[len++] = '*';
    search_path[len] = '\0';

    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        *out_count = 0;
        return NULL;
    }

    size_t capacity = 8;
    size_t count = 0;
    char **files = malloc(capacity * sizeof(char *));
    if (!files) {
        FindClose(hFind);
        *out_count = 0;
        return NULL;
    }

    do {
        if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (count == capacity) {
                capacity *= 2;
                char **temp = realloc(files, capacity * sizeof(char *));
                if (!temp) break;
                files = temp;
            }

            // Build full path manually
            char fullpath[MAX_PATH_];
            size_t i = 0, j = 0;
            while (folder[i] != '\0' && i < MAX_PATH_ - 1) {
                fullpath[i] = folder[i];
                i++;
            }
            if (i < MAX_PATH_ - 1) fullpath[i++] = '\\';
            while (find_data.cFileName[j] != '\0' && i < MAX_PATH_ - 1) {
                fullpath[i++] = find_data.cFileName[j++];
            }
            fullpath[i] = '\0';

            files[count++] = _strdup(fullpath);
        }
    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);
    *out_count = count;
    return files;
}

void free_files(char **files, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(files[i]);
    }
    free(files);
}

void index_dump_json(const InvertedIndex *inverted_index, const char *file_path) {
    Jim jim = {.pp = 4};  // Pretty print with 4 spaces

    jim_object_begin(&jim);

    // Document collection
    jim_member_key(&jim, "documents");
    jim_array_begin(&jim);
    for (size_t i = 0; i < inverted_index->collection.count; ++i) {
        jim_string(&jim, inverted_index->collection.items[i].content);
    }
    jim_array_end(&jim);

    // Terms index
    jim_member_key(&jim, "index");
    jim_object_begin(&jim);
    for (size_t i = 0; i < inverted_index->capacity; i++) {
        if (inverted_index->index[i].term.content != NULL) {
            // Term as key
            jim_member_key(&jim, inverted_index->index[i].term.content);

            // Postings list as array of objects
            jim_array_begin(&jim);
            for (size_t j = 0; j < inverted_index->index[i].doc_freq; j++) {
                jim_array_begin(&jim);
                jim_integer(&jim, inverted_index->index[i].items[j].doc_id);
                jim_integer(&jim, inverted_index->index[i].items[j].freq);
                jim_array_end(&jim);
            }
            jim_array_end(&jim);
        }
    }
    jim_object_end(&jim);

    // Statistics
    jim_member_key(&jim, "stats");
    jim_object_begin(&jim);
    jim_member_key(&jim, "document_count");
    jim_integer(&jim, inverted_index->collection.count);
    jim_member_key(&jim, "term_count");
    jim_integer(&jim, inverted_index->count);
    jim_object_end(&jim);

    jim_object_end(&jim);

    // Output the JSON
    FILE *file = fopen(file_path, "w");
    if (file == nullptr) {
        fprintf(stderr, "CAN NOT OPEN FILE: %s", file_path);
        return;
    }
    fwrite(jim.sink, jim.sink_count, 1, file);
}
