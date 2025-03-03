#include "handle-http.h"
#include "process-files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// socket headers
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

void http_handle_resquest(char * request, int socket) {
    if (strstr(request, "GET /")) {
        char * r_path;
        char * file_path;

        r_path = strtok(request, " ");
        r_path = strtok(NULL, " ");

        printf("get: %s\n", r_path);

        if (strcmp(r_path, "/") == 0) {
            http_send_file(socket, "./html/index.html");
        } else {
            // get if request has an extention
            char * ext = strrchr(r_path, '.');
            if (!ext || ext == r_path) {
                http_send_file(socket, "./html%s.html", r_path);
                printf("./html%s.html\n", r_path);
            } else {
                http_send_file(socket, "./html%s", r_path);
                //puts("ddddd");
                printf("./html%s\n", r_path);
            }
        }
    }


}

void http_send_file(int socket, const char * Path, ...) {
    // handle variadic arguments
    va_list args;
    va_start(args, Path);

    // get args size
    va_list args_copy;
    va_copy(args_copy, args);
    size_t size = vsnprintf(NULL, 0, Path, args_copy) + 1;
    va_end(args_copy);

    char *path = (char *)malloc(size * sizeof(char));
    if (path == NULL) {
        va_end(args);
    }

    vsnprintf(path, size, Path, args);
    va_end(args);


    size_t f_size;
    char * f_type = http_get_mime_type(path);
    char * data = get_file_data(&f_size, path);

    // if theres no file
    if (!data) {
        char * notfound = get_file_data(&f_size, "./html/404.html");

        char * r = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";

        send(socket, r, strlen(r), 0);
        send(socket, notfound, f_size, 0);
        send(socket, "\r\n", 2, 0);

        return;
    }


    // malloc size for response
    size_t r_size = strlen("HTTP/1.1 200 OK\r\nContent-Type: ") + strlen(f_type) + strlen("\r\nConnection: close\r\n\r\n") + 1;

    char * r = malloc(r_size);
    snprintf(r, r_size, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nConnection: close\r\n\r\n", f_type);

    // send file to browser
    send(socket, r, strlen(r), 0);
    send(socket, data, f_size, 0);
    send(socket, "\r\n", 2, 0);
}

// supports most of the common types
char * http_get_mime_type(const char * path) {
    char * ext = strrchr(path, '.');
    if (!ext || ext == path) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".wasm") == 0) return "application/wasm";

    if (strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".svg") == 0) return "image/svg+xml";
    if (strcmp(ext, ".ico") == 0) return "image/x-icon";

    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".pdf") == 0) return "application/pdf";
    if (strcmp(ext, ".xml") == 0) return "application/xml";

    if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    if (strcmp(ext, ".wav") == 0) return "audio/wav";
    if (strcmp(ext, ".flac") == 0) return "audio/flac";

    if (strcmp(ext, ".mp4") == 0) return "video/mp4";
    if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
    if (strcmp(ext, ".webm") == 0) return "video/webm";

    if (strcmp(ext, ".zip") == 0) return "application/zip";

    if (strcmp(ext, ".txt") == 0) return "text/plain";



    return "application/octet-stream";
}
