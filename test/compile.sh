gcc -I../src -c ../src/html_parser.c -o html_parser.o
gcc -I../src -o html_parser_test html_parser_test.c html_parser.o && ./html_parser_test