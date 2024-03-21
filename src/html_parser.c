#include "html_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int str_index(char *str, char *substr) {
    if (str == NULL || substr == NULL){
        return -1;
    }

    size_t len = strlen(str);
    size_t sublen = strlen(substr);
    if (len < sublen || sublen == 0) {
        return -1;
    }

    for (size_t i = 0; i < len - sublen; i++) {
        if (strncmp(str + i, substr, sublen) == 0) {
            return i;
        }
    }
    return -1;
}

#define COM_START "<!--"
#define COM_END "-->"

char *remove_comments(char *html) {
    size_t html_len = strlen(html);
    char *new_html = malloc(html_len + 1);

    int is_com = 0;
    size_t index = 0;

    for (size_t i = 0; i < html_len; i++) {
        if (is_com) {
            int cmp = strncmp(html + i, COM_END, strlen(COM_END));
            if (cmp == 0) {
                is_com = 0;
                i += strlen(COM_END) - 1;
            }
        } else {
            int cmp = strncmp(html + i, COM_START, strlen(COM_START));
            if (cmp == 0) {
                is_com = 1;
                i += strlen(COM_START) - 1;
            } else {
                new_html[index++] = html[i];
            }
        }
    }
    new_html[index] = '\0';
    return new_html;
}

#define CSS_LINK "<link rel=\"stylesheet\""

char* append_styles(char *html) {
    return NULL;
}

char* append_scripts(char *html) {
    return NULL;
}