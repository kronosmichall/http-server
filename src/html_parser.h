#include <stdbool.h>
#include <stdlib.h>
#define MAX_CSS_LINKS_IN_FILE 16

char* read_file(char *path, char *suffix);
int str_index(char *str, char *substr);
char *remove_comments(char *html);
struct html_parts append_styles(char *html, char *html_path);
void append_scripts(char *html);


struct html_parts {
    char *html[2 * MAX_CSS_LINKS_IN_FILE + 1];
    size_t size;
};

void html_parts_insert(struct html_parts *html_parts, char *html);
void free_html_parts(struct html_parts *html_parts);