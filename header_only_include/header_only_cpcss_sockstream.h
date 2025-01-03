#ifndef __cplusplus
#ifndef Included_header_only_cpcss_sockstream_h
#define Included_header_only_cpcss_sockstream_h
#ifndef _WIN32
#include <sys/select.h>
#include <time.h>
#endif

#include <cpcss_sockstream.h>

struct cpcss_transform_io
{   void*internal;
    void(*init)(void*,struct cpcss_socket_impl*);
    int(*read)(void*,char*,size_t);
    int(*write)(void*,const char*,size_t);
    int(*ready)(void*);
    int(*cleanup)(void*);   };

// open streams
struct cpcio____istream *cpcss_open_istream(struct cpcss_socket_impl *ss)
{   const struct cpcss_transform_io transformer = {cpcss_get_raw_socket(ss), &cpcss_noop_init, &cpcss_read_ss, &cpcss_write_ss, &cpcss_ready, &cpcss_close_ss};
    return cpcss_open_istream_ex(ss, &transformer);   }

struct cpcio____istream *cpcss_open_istream_ex(struct cpcss_socket_impl *ss,const struct cpcss_transform_io *trans)
{   cpcio_istream is = cpcio_open_istream(trans->internal, trans->read, trans->cleanup);
    is->ready = trans->ready;
    trans->init(trans->internal, ss);
    cpcio_toggle_buf_is(is);
    return is;   }

struct cpcio____ostream *cpcss_open_ostream(struct cpcss_socket_impl *ss)
{   const struct cpcss_transform_io transformer = {cpcss_get_raw_socket(ss), &cpcss_noop_init, &cpcss_read_ss, &cpcss_write_ss, &cpcss_ready, &cpcss_close_ss};
    return cpcss_open_ostream_ex(ss, &transformer);   }

struct cpcio____ostream *cpcss_open_ostream_ex(struct cpcss_socket_impl *ss,const struct cpcss_transform_io *trans)
{   cpcio_ostream os = cpcio_open_ostream(trans->internal, trans->write, trans->cleanup);
    trans->init(trans->internal, ss);
    cpcio_toggle_buf_os(os);
    return os;   }

int cpcss_ready(void *src)
{
#ifdef _WIN32
	return 0;
#else
	cpcss____sh *handlep = src, handle = *handlep;
	fd_set fds, *fdsp = &fds;
	FD_ZERO(fdsp);
	FD_SET(handle, fdsp);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	return select(handle + 1, fdsp, NULL, NULL, &tv);
#endif
}

int cpcss_read_ss(void *src,
                  char *arr,
                  size_t size)
{cpcss____sh *as = src;
#ifdef _WIN32
    return recv(*as, arr, size,0);
#else
    return read(*as, arr, size);
#endif
}

int cpcss_write_ss(void *dest,
                     const char *arr,
                     size_t size)
{   cpcss____sh *ad = dest; return send(*ad, arr, size, 0);   }

int cpcss_close_ss(void *stream)
{   return 0;   }

#endif //Included_header_only_sockstream_h
#endif //__cplusplus
