#ifndef HANDLEHTTP_H
#define HANDLEHTTP_H

extern void http_handle_resquest(char * request, int socket);
extern void http_send_file(int socket, const char * Path, ...);

extern char * http_get_mime_type(const char * path);

#endif
