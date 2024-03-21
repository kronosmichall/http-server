#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "html_parser.h"

void test_read_file() {
    char *file_str = read_file("html_parser.html");
    printf("file resulted in:\n");
    printf("%s\n", file_str);
    int result = strcmp(file_str, "<div>hello world</div>");
    free(file_str);

    assert(result == 0);
}

void test_remove_comments_simple() {
    char *html = "<!-- comment -->";
    char *new_html = remove_comments(html);
    printf("html resulted in: %s\n", new_html);
    assert (strcmp(new_html, "") == 0);
    free(new_html);
}

void test_remove_comments_from_file() {
    char *html = read_file("index.html");
    char *new_html = remove_comments(html);
    printf("html resulted in: \n%s\n", new_html);

    char *new_html2 = remove_comments(new_html);
    assert(strcmp(new_html, new_html2) == 0);

    free(new_html);
    free(new_html2);
}

int main() {
    test_read_file();
    test_remove_comments_simple();
    test_remove_comments_from_file();

    return 0;
}