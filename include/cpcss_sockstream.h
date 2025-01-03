#ifndef __cplusplus
#ifndef Included_cpcss_sockstream_h
#define Included_cpcss_sockstream_h

#ifndef HEADER_ONLY_CPC
#include <cpcio_istream.h>
#include <cpcio_ostream.h>
#else
#include <header_only_cpcio_istream.h>
#include <header_only_cpcio_ostream.h>
#endif
#include <cpcss_socket.h>

// structs used by the streams
struct cpcss____i_sock_src;
struct cpcss____o_sock_dest;

// functions used by the streams

// open client streams
struct cpcio____istream *open_client_istream(struct cpcss_socket_impl *ss);
struct cpcio____ostream *open_client_ostream(struct cpcss_socket_impl *ss);

// open server streams
struct cpcio____istream *open_server_istream(struct cpcss_socket_impl *cs);
struct cpcio____ostream *open_server_ostream(struct cpcss_socket_impl *cs);

// are there bytes that were sent but yet to be read
int cpcss_ready(void *src);

// reader
int cpcss_read_ss(void *src,
                    char *arr,
                    size_t size);

// writer
int cpcss_write_ss(void *dest,
                    const char *arr,
                    size_t size);

// closes the streams
int cpcss_close_ss(void *stream);

#endif //Included_sockstream_h
#endif
