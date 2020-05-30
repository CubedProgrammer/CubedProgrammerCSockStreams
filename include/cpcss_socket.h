#ifndef __cplusplus
#ifndef Included_cpcss_socket_h
#define Included_cpcss_socket_h

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <ws2tcpip.h>
#include <winsock2.h>

#elif defined __linux__

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

// structs for sockets
struct __cpcss_sv_sk;
struct __cpcss_cl_sk;

// functions for opening sockets
struct __cpcss_sv_sk* cpcss_open_server(const char *port);
struct __cpcss_cl_sk* cpcss_connect_client(const char *host,const char *port);

// typedefs for platforms
#ifdef _WIN32

typedef SOCKET __sh;
typedef struct addrinfo __sa;

#elif defined __linux__

typedef int __sh;
typedef struct sockaddr_in __sa;

#endif

// functions for getting members of structs
__sh *cpcss_client_socket_get_server(struct __cpcss_cl_sk *c);
__sh *cpcss_server_socket_get_client(struct __cpcss_sv_sk *s);

// typedefs
typedef struct __cpcss_sv_sk* cpcss_server_sock;
typedef struct __cpcss_cl_sk* cpcss_client_sock;

#endif //Included_cpcss_socket_h
#endif