//
// Created by VICTUS on 7/22/2025.
//

#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdlib.h>
#include "counter.h"

char **get_files_in_folder(const char *folder, size_t *out_count);

void free_files(char **files, size_t count);

void index_dump_json(const InvertedIndex *inverted_index, const char *file_path);

#endif // FILE_UTIL_H
