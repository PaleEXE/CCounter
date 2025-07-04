#include "counter.h"
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

char **get_files_in_folder(const char *folder, size_t *out_count) {
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", folder);

    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open folder: %s\n", folder);
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
            // Resize array if needed
            if (count == capacity) {
                capacity *= 2;
                char **temp = realloc(files, capacity * sizeof(char *));
                if (!temp) {
                    fprintf(stderr, "Memory error\n");
                    break;
                }
                files = temp;
            }

            // Build full path
            char fullpath[MAX_PATH];
            snprintf(fullpath, MAX_PATH, "%s\\%s", folder, find_data.cFileName);

            files[count++] = _strdup(fullpath);
        }
    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);
    *out_count = count;
    putchar('\n');
    return files;
}

void free_files(char **files, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(files[i]);
    }
    free(files);
}

int main(void) {
    char **files;
    size_t count;

    files = get_files_in_folder("..\\songs", &count);
    for (size_t i = 0; i < count; ++i) {
        printf("File: %s\n", files[i]);
    }


    InvertedIndex inverted_index = inverted_index_new();

    for (size_t i = 0; i < count; ++i) {
        add_document(&inverted_index, files[i]);
    }
    index_print(&inverted_index);
    free_files(files, count);
    return 0;
}
