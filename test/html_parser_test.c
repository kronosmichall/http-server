#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "html_parser.h"

#define EQ(expected, actual) \
    do { \
        if (actual != expected) { \
            fprintf(stderr, "%s:%d: Assertion failed:\n Expected %d and got %d\n", __FILE__, __LINE__, expected, actual); \
            exit(0); \
        } \
    } while (0)

#define NEQ(expected, actual) \
    do { \
        if (actual == expected) { \
            fprintf(stderr, "%s:%d: Assertion failed:\n Expected not %d \n", __FILE__, __LINE__, expected); \
            exit(0); \
        } \
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
    char no_style[] = "<div>hello world</div>";
    append_styles(no_style, NULL);
    printf("no style resulted in: %s\n", no_style);

    char style[] = "lala <link rel=\"stylesheet\" href=\"assets/css/noscript.css\" /> lala";
    append_styles(style, "index/");
    printf("style resulted in: %s\n", style);
}

int main() {
    test_read_file();
    test_str_index();
    test_remove_comments_simple();
    test_remove_comments_from_file();
    test_append_styles_simple();

    return 0;
}