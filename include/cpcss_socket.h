#ifndef __cplusplus
#ifndef Included_cpcss_socket_h
#define Included_cpcss_socket_h

#include <stdint.h>
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
struct cpcss_socket_impl;
// functions for opening sockets
struct cpcss_socket_impl* cpcss_open_server_ex(uint16_t port,int v6);
struct cpcss_socket_impl* cpcss_open_server(uint16_t port);
struct cpcss_socket_impl* cpcss_accept_client(struct cpcss_socket_impl *sv);
struct cpcss_socket_impl* cpcss_connect_client(const char *host,uint16_t port);
struct cpcss_socket_impl* cpcss_connect_client_ex(const char *host,uint16_t port,int v6);

// closing sockets
int cpcss_close_server(struct cpcss_socket_impl *sv);
int cpcss_discon_client(struct cpcss_socket_impl *cs);

// typedefs for platforms
#ifdef _WIN32
typedef SOCKET cpcss____sh;
#else
typedef int cpcss____sh;
#endif
typedef struct sockaddr_in cpcss____sa;

// functions for getting members of structs
cpcss____sh *cpcss_get_raw_socket(struct cpcss_socket_impl *socket);
cpcss____sh *cpcss_client_socket_get_server(struct cpcss_socket_impl *c);
cpcss____sh *cpcss_server_socket_get_server(struct cpcss_socket_impl *s);

// typedefs
typedef struct cpcss_socket_impl* cpcss_socket;
typedef cpcss_socket cpcss_server_sock;
typedef cpcss_socket cpcss_client_sock;

#endif //Included_cpcss_socket_h
#endif
