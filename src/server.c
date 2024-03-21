#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "ph/thpool.h"
#include "html_parser.h"

#define PORT 8080
#define BUFFER_SIZE 1024

void terminate(char *message) {
    perror(message);
    exit(0);
}

struct serve_info {
    u_int8_t free;
    int sockfd;
    socklen_t host_addr_len;
    socklen_t client_addr_len;
    struct sockaddr_in host_addr;
    struct sockaddr_in client_addr;
    char buffer[BUFFER_SIZE];
};

void quit_job(struct serve_info* serve_info, int sockfd, char *message) {
    perror(message);
    close(sockfd);
    serve_info->free = 1;
}

void serve_job(struct serve_info* serve_info) {
    memset(serve_info->buffer, 0, BUFFER_SIZE);
    int newsockfd = accept(serve_info->sockfd, (struct sockaddr*) &serve_info->host_addr, (socklen_t *) &serve_info->host_addr_len);
    if (newsockfd < 0) {
        quit_job(serve_info, newsockfd, "server: connecton not accepted");
        return;
    }

    int sockn = getsockname(newsockfd, (struct sockaddr*) &serve_info->client_addr, (socklen_t *) &serve_info->client_addr_len);
    if (sockn == -1) {
        quit_job(serve_info, newsockfd, "server: getsockname failed");
        return;
    }
    printf("[%s:%u]\n", inet_ntoa(serve_info->client_addr.sin_addr), ntohs(serve_info->client_addr.sin_port));

    ssize_t read_res = read(newsockfd, serve_info->buffer, BUFFER_SIZE);
    if (read_res < 0) {
        quit_job(serve_info, newsockfd, "server: read failed");
        return;
    }

    char resp[] =   "HTTP/1.0 200 OK\r\n"
                    "Server: webserver-c\r\n"
                    "Content-type: text/html\r\n\r\n";
    ssize_t write_res = write(newsockfd, resp, strlen(resp));
    if (write_res < 0) {
        perror("server: write failed");
        return;
    }
    
    char *html_str = read_file("../src/html/index/index.html");
    write_res = write(newsockfd, html_str, strlen(html_str));
    free(html_str);
    if (write_res < 0) {
        perror("server: write failed");
        return;
    }
    quit_job(serve_info, newsockfd, "server: connection accepted");
}

static volatile int keepRunning = 1;
void handle_sigint(int) {
    keepRunning = 0;
}

int main(int argc, char** argv) {
    if (argc != 1 && argc != 3) {
        terminate("Wrong amount of args!");
    }

    u_int8_t thread_count = 8; 
    if (argc != 1 && strcmp(argv[1], "-t") == 0) {
        thread_count = atoi(argv[2]);
    }
    printf("server: building with %d threads\n", thread_count);

    // socket file descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        terminate("server: socket failed");
    }
    printf("server: socked created\n");

    struct sockaddr_in host_addr;
    socklen_t host_addr_len = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    // convert short to big endian
    host_addr.sin_port = htons(PORT);
    // convert int to big endian
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_res = bind(sockfd, (struct sockaddr*) &host_addr, host_addr_len);
    if (bind_res == -1) {
        terminate("server: bind failed");
    }
    signal(SIGINT, handle_sigint);
    
    printf("server: socked succesfully bound to address\n");

    int listen_res = listen(sockfd, SOMAXCONN);
    if (listen_res == -1) {
        terminate("server: listen failed");
    }
    printf("server: listening for connections\n");

    threadpool thpool = thpool_init(thread_count);
    struct serve_info serve_infos[thread_count];
    for (u_int8_t i = 0; i < thread_count; i++) {
        serve_infos[i].free = 1;
        serve_infos[i].sockfd = sockfd;
        serve_infos[i].host_addr_len = host_addr_len;
        serve_infos[i].client_addr_len = sizeof(struct sockaddr_in);
        serve_infos[i].host_addr = host_addr;
    }

    uint8_t thread_i = 0;
    while (keepRunning) {
        while (keepRunning && serve_infos[thread_i].free == 0) {
            thread_i++;
            if (thread_i == thread_count) {
                thread_i = 0;
            }
        }
        serve_infos[thread_i].free = 0;
        thpool_add_work(thpool, (void *)serve_job, &serve_infos[thread_i]);
    }

    close(sockfd);
    return 0;
}