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

// struct for transformed IO
struct cpcss_transform_io
{   void*internal;
    void(*init)(void*,struct cpcss_socket_impl*);
    int(*read)(void*,char*,size_t);
    int(*write)(void*,const char*,size_t);
    int(*ready)(void*);
    int(*cleanup)(void*);   };

// open streams
struct cpcio____istream *cpcss_open_istream(struct cpcss_socket_impl *ss);
struct cpcio____istream *cpcss_open_istream_ex(struct cpcss_socket_impl *ss,const struct cpcss_transform_io *trans);

struct cpcio____ostream *cpcss_open_ostream(struct cpcss_socket_impl *ss);
struct cpcio____ostream *cpcss_open_ostream_ex(struct cpcss_socket_impl *ss,const struct cpcss_transform_io *trans);

// are there bytes that were sent but yet to be read
int cpcss_ready(void *src);

// no-op init function
static inline void cpcss_noop_init(void*,struct cpcss_socket_impl*)
{}

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
