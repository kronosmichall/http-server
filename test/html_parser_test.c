#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "html_parser.h"

#define EQ(expected, actual)                                                                                              \
    do {                                                                                                                  \
        if (actual != expected) {                                                                                         \
            fprintf(stderr, "%s:%d: Assertion failed:\n Expected %d and got %d\n", __FILE__, __LINE__, expected, actual); \
            exit(0);                                                                                                      \
        }                                                                                                                 \
    } while (0)

#define NEQ(expected, actual)                                                                               \
    do {                                                                                                    \
        if (actual == expected) {                                                                           \
            fprintf(stderr, "%s:%d: Assertion failed:\n Expected not %d \n", __FILE__, __LINE__, expected); \
            exit(0);                                                                                        \
        }                                                                                                   \
    } while (0)

void test_read_file() {
    char *file_str = read_file("index.html", "index/");
    assert(file_str != NULL);
    int result = strcmp(file_str, "<div>hello world</div>");
    free(file_str);

    NEQ(0, result);
}

void test_str_index() {
    int i = str_index("hello world", "world");
    EQ(6, i);
    i = str_index("hello world", "hello");
    EQ(0, i);
    i = str_index("hello world", "hello world");
    EQ(0, i);
    i = str_index("hello world", "hello world!");
    EQ(-1, i);
    i = str_index("hello world", "");
    EQ(-1, i);
    i = str_index("", "hello world");
    EQ(-1, i);
    i = str_index("", "");
    EQ(-1, i);
    i = str_index("hello world", "he??");
    EQ(-1, i);
}

void test_remove_comments_simple() {
    char *html = "<!-- comment -->";
    char *new_html = remove_comments(html);
    printf("html resulted in: %s\n", new_html);
    EQ(0, strcmp(new_html, ""));
    free(new_html);
}

void test_remove_comments_from_file() {
    char *html = read_file("index.html", "index/");
    char *new_html = remove_comments(html);
    printf("html resulted in: \n%s\n", new_html);

    char *new_html2 = remove_comments(new_html);
    EQ(0, strcmp(new_html, new_html2));

    free(new_html);
    free(new_html2);
}

void test_append_styles_simple() {
    char *no_style_str = "<div>hello world</div>";
    char *no_style_html = calloc(1, strlen(no_style_str) + 1);
    strcpy(no_style_html, no_style_str);

    struct html_parts no_style = append_styles(no_style_html, NULL);
    EQ(1, no_style.size);
    EQ(0, strcmp(no_style.html[0], no_style_str));

    for (int i = 0; i < no_style.size; i++) {
        printf("no_style html[%d]: %s\n", i, no_style.html[i]);
    }

    char *style_str = "lala<link rel=\"stylesheet\" href=\"assets/css/noscript.css\" />lala";
    char *style_html = calloc(1, strlen(style_str) + 1);
    strcpy(style_html, style_str);
    struct html_parts style = append_styles(style_html, "index/");

    for (int i = 0; i < style.size; i++) {
        printf("style html[%d]: %s\n", i, style.html[i]);
    }
    
    EQ(3, style.size);
    EQ(0, strcmp(style.html[0], "lala"));
    EQ(0, strcmp(style.html[2], "lala"));

    free_html_parts(&no_style);
    free_html_parts(&style);
}

int main() {
    test_read_file();
    test_str_index();
    test_remove_comments_simple();
    test_remove_comments_from_file();
    test_append_styles_simple();

    return 0;
}