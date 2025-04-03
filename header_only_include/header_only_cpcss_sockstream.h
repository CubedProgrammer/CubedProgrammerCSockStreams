#ifndef __cplusplus
#ifndef Included_header_only_cpcss_sockstream_h
#define Included_header_only_cpcss_sockstream_h
#ifndef _WIN32
#include <sys/select.h>
#include <time.h>
#endif

#include <cpcss_sockstream.h>

// open streams
struct cpcio____istream *cpcss_open_istream(struct cpcss_socket_impl *ss)
{   const struct cpcss_transform_io transformer = {cpcss_get_raw_socket(ss), &cpcss_noop_init, &cpcss_read_ss, &cpcss_write_ss, &cpcss_ready, &cpcss_select, &cpcss_close_ss};
    return cpcss_open_istream_ex(ss, &transformer);   }

struct cpcio____istream *cpcss_open_istream_ex(struct cpcss_socket_impl *ss,const struct cpcss_transform_io *trans)
{   cpcio_istream is = cpcio_open_istream(trans->internal, trans->read, trans->cleanup);
    is->ready = trans->ready;
    is->select = trans->select;
    trans->init(trans->internal, ss);
    cpcio_toggle_buf_is(is);
    return is;   }

struct cpcio____ostream *cpcss_open_ostream(struct cpcss_socket_impl *ss)
{   const struct cpcss_transform_io transformer = {cpcss_get_raw_socket(ss), &cpcss_noop_init, &cpcss_read_ss, &cpcss_write_ss, &cpcss_ready, &cpcss_select, &cpcss_close_ss};
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

int cpcss_select(void **first, void **last, long *ms)
{   fd_set fds;
    struct timeval tv = {*ms / 1000, *ms % 1000 * 1000};
    int big = 0;
    FD_ZERO(&fds);
    for(void** it = first; it != last; ++it)
    {
#ifdef _WIN32
#else
    	big = *(cpcss____sh*)*it > big ? *(cpcss____sh*)*it : big;
        FD_SET(*(cpcss____sh*)*it, &fds);
#endif
    }
    int ready = select(big + 1, &fds, NULL, NULL, &tv);
    for(cpcss____sh**it = (cpcss____sh**)first; it != (cpcss____sh**)last; ++it)
    {   if(!FD_ISSET(**it, &fds))
           *it = NULL;   }
    *ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return ready;   }

size_t cpcss_read_ss(void *src,
                     void *arr,
                     size_t size)
{cpcss____sh *as = src;
#ifdef _WIN32
    return recv(*as, arr, size,0);
#else
    return read(*as, arr, size);
#endif
}

size_t cpcss_write_ss(void *dest,
                      const void *arr,
                      size_t size)
{   cpcss____sh *ad = dest; return send(*ad, arr, size, 0);   }

int cpcss_close_ss(void *stream)
{   return 0;   }

#endif //Included_header_only_sockstream_h
#endif //__cplusplus
