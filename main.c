#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// socket headers
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

//#include "process-files.h"
#include "handle-http.h"

#define PORT 80

int server_fd, client_fd;
struct sockaddr_in server_address;
int addr_size = 0;
int opt = 1;

void init_html_pages(void);

void socket_init(void);
void handle_connection(void);

int main(void) {
    socket_init();
    handle_connection();

    close(server_fd);
    close(client_fd);
    return 0;
}

void handle_connection(void) {
    char buff[4048]; // since the server is only handling GET requests, maybe the buffer dosen't need to be big

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&server_address, (socklen_t*)&addr_size);
        puts("new connection");
        if (client_fd == -1) {
            perror("client socket error");
            close(client_fd);
            exit(20);
        }

        memset(buff, 0, sizeof(buff));
        int bytes_recv = recv(client_fd, buff, sizeof(buff), 0);

        if (bytes_recv < 0) {
            perror("recieved no bytes from client");
            close(client_fd);
            exit(21);
        }

        printf("\n%s\n", buff);

        http_handle_resquest(buff, client_fd);
        close(client_fd);
    }
}

void socket_init(void) {
    // make socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "error creating socket");
        exit(10);
    }
    // set server socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        fprintf(stderr, "error socket options");
        exit(11);
    }


    addr_size = sizeof(server_address);

    // set server address
    server_address.sin_family = AF_INET;
    // accept connections on localhost
    server_address.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY
    server_address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "error binding server\n");
        close(server_fd);
        exit(12);
    }
    if (listen(server_fd, 4) < 0) {
        fprintf(stderr, "error on listen()");
        close(server_fd);
        exit(13);
    }

    // print what port server is in
    printf("server on port: %d\n", PORT);

}

