//
// Created by VICTUS on 7/22/2025.
//

#include "file_util.h"
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "thirdparty/nob.h"
#define JIM_IMPLEMENTATION
#define JIMP_IMPLEMENTATION
#include "jim/jim.h"
#include "counter.h"
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define LL long long
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
        return nullptr;
    }

    size_t capacity = 8;
    size_t count = 0;
    char **files = malloc(capacity * sizeof(char *));
    if (!files) {
        FindClose(hFind);
        *out_count = 0;
        return nullptr;
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
    Jim jim = {
        .pp = 4
    }; // Pretty print with 4 spaces

    jim_object_begin(&jim);

    // Statistics
    jim_member_key(&jim, "document_count");
    jim_integer(&jim, (LL) inverted_index->collection.count);
    jim_member_key(&jim, "term_count");
    jim_integer(&jim, (LL) inverted_index->count);

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
        if (inverted_index->index[i].term.content != nullptr) {
            // Term as key
            jim_member_key(&jim, inverted_index->index[i].term.content);

            // Postings list as array of objects
            jim_array_begin(&jim);
            for (size_t j = 0; j < inverted_index->index[i].doc_freq; j++) {
                jim_array_begin(&jim);
                jim_integer(&jim, (LL) inverted_index->index[i].items[j].doc_id);
                jim_integer(&jim, (LL) inverted_index->index[i].items[j].freq);
                jim_array_end(&jim);
            }
            jim_array_end(&jim);
        }
    }
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

bool index_load_json(InvertedIndex *inverted_index, const char *file_path) {
    String_Builder sb = {0};
    Jimp jimp = {.string_capacity = 0};
    Str temp;

    if (!read_entire_file(file_path, &sb)) return false;

    jimp_begin(&jimp, file_path, sb.items, sb.count);

    // Start of main object
    if (!jimp_object_begin(&jimp)) {
        fprintf(stderr, "Expected object start `{` but got `%s`\n", jimp.string);
        return false;
    }

    // Parse document_count
    jimp_object_member(&jimp);
    if (strcmp(jimp.string, "document_count") != 0) {
        fprintf(stderr, "Expected `document_count` but got `%s`\n", jimp.string);
        return false;
    }
    if (!jimp_number(&jimp)) {
        fprintf(stderr, "Expected number for `document_count` but got `%s`\n", jimp.string);
        return false;
    }
    inverted_index->collection = list_with_capacity((size_t) jimp.number);

    // Parse term_count
    jimp_object_member(&jimp);
    if (strcmp(jimp.string, "term_count") != 0) {
        fprintf(stderr, "Expected `term_count` but got `%s`\n", jimp.string);
        return false;
    }
    if (!jimp_number(&jimp)) {
        fprintf(stderr, "Expected number for `term_count` but got `%s`\n", jimp.string);
        return false;
    }
    inverted_index->count = (size_t) jimp.number;
    inverted_index->capacity = (size_t) jimp.number * 2; // Typical hash table sizing
    inverted_index->index = calloc(inverted_index->capacity, sizeof(*inverted_index->index));

    // Parse documents array
    jimp_object_member(&jimp);
    if (strcmp(jimp.string, "documents") != 0) {
        fprintf(stderr, "Expected `documents` but got `%s`\n", jimp.string);
        return false;
    }
    if (!jimp_array_begin(&jimp)) {
        fprintf(stderr, "Expected array start `[` after `documents` but got `%s`\n", jimp.string);
        return false;
    }
    while (jimp_array_item(&jimp)) {
        if (!jimp_string(&jimp)) {
            fprintf(stderr, "Expected document content string but got `%s`\n", jimp.string);
            return false;
        }
        temp = to_str(strdup(jimp.string));
        append(&inverted_index->collection, temp);
    }
    if (!jimp_array_end(&jimp)) {
        fprintf(stderr, "Expected array end `]` after documents but got `%s`\n", jimp.string);
        return false;
    }

    // Parse index object
    jimp_object_member(&jimp);
    if (strcmp(jimp.string, "index") != 0) {
        fprintf(stderr, "Expected `index` but got `%s`\n", jimp.string);
        return false;
    }
    if (!jimp_object_begin(&jimp)) {
        fprintf(stderr, "Expected object start `{` after `index` but got `%s`\n", jimp.string);
        return false;
    }

    while (jimp_object_member(&jimp)) {
        // Term is the key
        temp = to_str(strdup(jimp.string));

        // Start of postings list array
        if (!jimp_array_begin(&jimp)) {
            fprintf(stderr, "Expected array start `[` for term `%s` but got `%s`\n", temp.content, jimp.string);
            return false;
        }

        while (jimp_array_item(&jimp)) {
            // Start of posting pair array [doc_id, freq]
            if (!jimp_array_begin(&jimp)) {
                fprintf(stderr, "Expected array start `[` for posting of term `%s` but got `%s`\n", temp.content,
                        jimp.string);
                return false;
            }

            // Parse doc_id
            if (!jimp_array_item(&jimp)) {
                fprintf(stderr, "Expected document ID for term `%s` but got `%s`\n", temp.content, jimp.string);
                return false;
            }
            if (!jimp_number(&jimp)) {
                fprintf(stderr, "Expected number for document ID but got `%s`\n", jimp.string);
                return false;
            }
            size_t doc_id = (size_t) jimp.number;

            // Parse term frequency
            if (!jimp_array_item(&jimp)) {
                fprintf(stderr, "Expected term frequency for term `%s` but got `%s`\n", temp.content, jimp.string);
                return false;
            }
            if (!jimp_number(&jimp)) {
                fprintf(stderr, "Expected number for term frequency but got `%s`\n", jimp.string);
                return false;
            }
            size_t term_freq = (size_t) jimp.number;

            if (!jimp_array_end(&jimp)) {
                fprintf(stderr, "Expected array end `]` for posting of term `%s` but got `%s`\n", temp.content,
                        jimp.string);
                return false;
            }

            // Add to index
            append_index(inverted_index, &temp, doc_id, term_freq);
        }

        if (!jimp_array_end(&jimp)) {
            fprintf(stderr, "Expected array end `]` for term `%s` but got `%s`\n", temp.content, jimp.string);
            return false;
        }
    }

    if (!jimp_object_end(&jimp)) {
        fprintf(stderr, "Expected object end `}` for index but got `%s`\n", jimp.string);
        return false;
    }

    if (!jimp_object_end(&jimp)) {
        fprintf(stderr, "Expected final object end `}` but got `%s`\n", jimp.string);
        return false;
    }

    return true;
}
