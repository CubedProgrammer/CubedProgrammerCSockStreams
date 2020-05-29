#ifndef __cplusplus
#ifndef Included_sockstream_h
#define Included_sockstream_h

#include <istream.h>
#include <ostream.h>
#include <cpcss_socket.h>

// structs used by the streams
struct __i_sock_src;
struct __o_sock_dest;

// functions used by the streams

// open client streams
struct __istream *open_client_istream(struct __cpcss_sv_sk *ss);
struct __ostream *open_client_ostream(struct __cpcss_sv_sk *ss);

// open server streams
struct __istream *open_server_istream(struct __cpcss_cl_sk *cs);
struct __ostream *open_server_ostream(struct __cpcss_cl_sk *cs);

// reader
int __cpcss_read_ss(void *src,
                    char *arr,
                    size_t size);

// writer
int __cpcss_write_ss(void *dest,
                    const char *arr,
                    size_t size);

// closes the streams
int __cpcss_close_ss(void *stream);

#endif //Included_sockstream_h
#endif