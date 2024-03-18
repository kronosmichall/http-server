#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void terminate(char *message) {
    perror(message);
    exit(1);
}

int main() {
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

    int bind_return_code = bind(sockfd, (struct sockaddr*) &host_addr, host_addr_len);
    if (bind_return_code == -1) {
        terminate("server: bind failed");
    }
    printf("server: socked succesfully bound to address\n");

    int listen_return_code = listen(sockfd, SOMAXCONN);
    if (listen_return_code == -1) {
        terminate("server: listen failed");
    }
    printf("server: listening for connections\n");

    char buffer[BUFFER_SIZE];
    for (;;) {
        memset(buffer, 0, BUFFER_SIZE);
        int newsockfd = accept(sockfd, (struct sockaddr*) &host_addr, (socklen_t *) &host_addr_len);
        if (newsockfd < 0) {
            perror("server: connecton not accepted");
            continue;
        }

        ssize_t read_res = read(newsockfd, buffer, BUFFER_SIZE);
        if (read_res < 0) {
            perror("server: read failed");
            continue;
        }

        char resp[] =   "HTTP/1.0 200 OK\r\n"
                        "Server: webserver-c\r\n"
                        "Content-type: text/html\r\n\r\n"
                        "<html>hello, world</html>\r\n";
        ssize_t write_res = write(newsockfd, resp, strlen(resp));
        if (write_res < 0) {
            perror("server: write failed");
            continue;
        }


        printf("server: connection accepted\n");
        close(newsockfd);
    }

    return 0;
}