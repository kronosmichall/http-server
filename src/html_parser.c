#include "html_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTML_PATH "/httpServer/src/html/"

char *read_file(char *path, char *suffix) {
    FILE *f;
    char *full_path_str;
    char *relative_path;

    if (suffix == NULL) {
        relative_path = "";
    } else {
        relative_path = suffix;
    }

    char *home_dir = getenv("HOME");
    full_path_str = calloc(1, strlen(home_dir) + strlen(HTML_PATH) + strlen(relative_path) + strlen(path) + 1);
    strcpy(full_path_str, home_dir);
    strcat(full_path_str, HTML_PATH);
    strcat(full_path_str, relative_path);
    strcat(full_path_str, path);

    printf("opened file %s\n", full_path_str);
    f = fopen(full_path_str, "rb");
    if (!f) {   
        printf("Cannot open file: %s\n", full_path_str);
        free(full_path_str);
        return NULL;
    }
    free(full_path_str);

    fseek(f, 0, SEEK_END);
    long flen = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = calloc(1, flen + 1);
    if (buffer == NULL) {
        printf("Cannot allocate %ld memory for the buffer\n", flen + 1);
        fclose(f);
        return NULL;
    }
    fread(buffer, 1, flen, f);
    buffer[flen] = '\0';
    fclose(f);

    return buffer;
}

int str_index(char *str, char *substr) {
    if (str == NULL || substr == NULL) {
        return -1;
    }

    size_t len = strlen(str);
    size_t sublen = strlen(substr);
    if (len < sublen || sublen == 0) {
        return -1;
    }

    for (size_t i = 0; i < len - sublen + 1; i++) {
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
#define SPACE ' '
#define HREF "href=\""

// to jest w sumie kwadratowe - trzeba zrobić struct string który będzie miał długość i wskaźnik na char*
void append_styles(char *html, char *html_path) {
    size_t html_len = strlen(html);
    int i = 0, css_start, css_end, href_start, href_end;
    char *css_path, *css_str;

    while (i < html_len) {
        css_start = str_index(html + i, CSS_LINK);
        if (css_start == -1) {
            return;
        }
        css_end = str_index(html + i + css_start, ">") + 1;
        if (css_end == -1) {
            return;
        }
        href_start = str_index(html + i, HREF);
        if (href_start == -1) {
            return;
        }
        href_end = str_index(html + i + href_start + strlen(HREF), "\"");
        if (href_end == -1) {
            return;
        }

        css_path = calloc(1, href_end + 1);
        memcpy(css_path, html + i + href_start + strlen(HREF), href_end);
        printf("css_path %s\n", css_path);

        css_str = read_file(css_path, "index/");
        if (css_str == NULL) {
            return;
        }

        printf("css style %s\n", css_str);

        free(css_str);
        free(css_path);

        printf("html len: %ld\n", html_len);
        printf("i: %d\n", i);
        printf("css_start: %d, css_end: %d\n", css_start, css_end);

        
        // memset(html + i + css_start, SPACE, css_end);
        i = css_end;
    }
}

void append_scripts(char *html) {
}