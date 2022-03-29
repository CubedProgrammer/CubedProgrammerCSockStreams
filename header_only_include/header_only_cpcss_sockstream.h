#ifndef __cplusplus
#ifndef Included_header_only_cpcss_sockstream_h
#define Included_header_only_cpcss_sockstream_h
#ifndef _WIN32
#include <sys/select.h>
#include <time.h>
#endif

#include <cpcss_sockstream.h>

// open client streams
struct cpcio____istream *open_client_istream(struct cpcss____ss *ss)
{   cpcio_istream is = cpcio_open_istream(cpcss_server_socket_get_client(ss), &cpcss_read_ss, &cpcss_close_ss);
    is->ready = &cpcss_ready;
    cpcio_toggle_buf_is(is);
    return is;   }

struct cpcio____ostream *open_client_ostream(struct cpcss____ss *ss)
{   cpcio_ostream os = cpcio_open_ostream(cpcss_server_socket_get_client(ss), &cpcss_write_ss, &cpcss_close_ss);
    cpcio_toggle_buf_os(os);
    return os;   }

// open client streams
struct cpcio____istream *open_server_istream(struct cpcss____cs *cs)
{   cpcio_istream is = cpcio_open_istream(cpcss_client_socket_get_server(cs), &cpcss_read_ss, &cpcss_close_ss);
    is->ready = &cpcss_ready;
    cpcio_toggle_buf_is(is);
    return is;   }

struct cpcio____ostream *open_server_ostream(struct cpcss____cs *cs)
{   cpcio_ostream os = cpcio_open_ostream(cpcss_client_socket_get_server(cs), &cpcss_write_ss, &cpcss_close_ss);
    cpcio_toggle_buf_os(os);
    return os;   }

int cpcss_ready(void *src)
{
#ifdef _WIN32
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
