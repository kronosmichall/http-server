#include "html_parser.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file(char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        printf("Cannot open file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long flen = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = calloc(1, flen + 1);
    if (buffer == NULL) {
        printf("Cannot allocate memory for the buffer\n");
        fclose(f);
        return NULL;
    }
    fread(buffer, 1, flen, f);
    buffer[flen] = '\0';
    fclose(f);

    return buffer;
}