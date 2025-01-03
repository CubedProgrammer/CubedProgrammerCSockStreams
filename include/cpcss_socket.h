#ifndef __cplusplus
#ifndef Included_cpcss_socket_h
#define Included_cpcss_socket_h

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <sys/socket.h>
#include <unistd.h>

#endif

// structs for sockets
// server socket and client socket
struct cpcss____ss;
struct cpcss____cs;

// functions for opening sockets
struct cpcss____ss* cpcss_open_server(const char *port);
struct cpcss____cs* cpcss_accept_client(struct cpcss____ss *sv);
struct cpcss____cs* cpcss_connect_client(const char *host,const char *port);

// closing sockets
int cpcss_close_server(struct cpcss____ss *sv);
int cpcss_discon_client(struct cpcss____cs *cs);

// typedefs for platforms
#ifdef _WIN32

typedef SOCKET cpcss____sh;
typedef struct addrinfo cpcss____sa;

#else

typedef int cpcss____sh;
typedef struct sockaddr_in cpcss____sa;

#endif

// functions for getting members of structs
cpcss____sh *cpcss_client_socket_get_server(struct cpcss____cs *c);
cpcss____sh *cpcss_server_socket_get_server(struct cpcss____ss *s);

// typedefs
typedef struct cpcss____ss* cpcss_server_sock;
typedef struct cpcss____cs* cpcss_client_sock;

#endif //Included_cpcss_socket_h
#endif
