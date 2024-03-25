#include <stdbool.h>
char* read_file(char *path, char *suffix);
int str_index(char *str, char *substr);
char *remove_comments(char *html);
void append_styles(char *html, char *html_path);
void append_scripts(char *html);
