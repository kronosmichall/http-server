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
#define STYLE_TAG "<style>"
#define STYLE_TAG_END "</style>"

// to jest w sumie kwadratowe - trzeba zrobić struct string który będzie miał długość i wskaźnik na char*
struct html_parts append_styles(char *html, char *html_path) {
    struct html_parts html_parts = {
        .html = { NULL },
        .size = 0
    };
    size_t html_len = strlen(html);
    int i = 0, css_start, css_end, href_start, href_end;
    char *css_path, *css_str;

    while (i < html_len) {
        css_start = str_index(html + i, CSS_LINK);
        if (css_start == -1) break;

        css_end = str_index(html + i + css_start, ">") + 1;
        if (css_end == -1) break;

        href_start = str_index(html + i, HREF);
        if (href_start == -1) break;

        href_end = str_index(html + i + href_start + strlen(HREF), "\"");
        if (href_end == -1) break;

        css_path = calloc(1, href_end + 1);
        memcpy(css_path, html + i + href_start + strlen(HREF), href_end);

        css_str = read_file(css_path, "index/");
        if (css_str == NULL) break;

        char *css_str_styled = calloc(1, strlen(css_str) + strlen(STYLE_TAG) + strlen(STYLE_TAG_END) + 1);
        strcat(css_str_styled, STYLE_TAG);
        strcat(css_str_styled, css_str);
        strcat(css_str_styled, STYLE_TAG_END);
        free(css_str);

        printf("css_path %s\n", css_path);
        printf("css_str %s\n", css_str);
        printf("html len: %ld\n", html_len);
        printf("i: %d\n", i);
        printf("css_start: %d, css_end: %d\n", css_start, css_end);
        
        char *html_part = calloc(1, html_len - i - css_start);
        memcpy(html_part, html + i, css_start);
        html_parts_insert(&html_parts, html_part);
        html_parts_insert(&html_parts, css_str_styled);
        free(css_path);

        i += css_start + css_end;
    }

    if (i < html_len) {
        char *html_part = calloc(1, strlen(html + i) + 1);
        strcpy(html_part, html + i);
        html_parts_insert(&html_parts, html_part);
    }
    return html_parts;
}

void append_scripts(char *html) {
}

void html_parts_insert(struct html_parts *html_parts, char *html) {
    if (html_parts->size >= 2 * MAX_CSS_LINKS_IN_FILE + 1) {
        return;
    }
    html_parts->html[html_parts->size++] = html;
}

void free_html_parts(struct html_parts *html_parts) {
    for (size_t i = 0; i < html_parts->size; i++) {
        free(html_parts->html[i]);
        html_parts->html[i] = NULL;
    }
    html_parts->size = 0;
}