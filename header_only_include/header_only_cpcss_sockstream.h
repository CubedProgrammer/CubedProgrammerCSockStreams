#ifndef __cplusplus
#ifndef Included_header_only_cpcss_sockstream_h
#define Included_header_only_cpcss_sockstream_h

#include <cpcss_sockstream.h>

// open client streams
struct cpcio____istream *open_client_istream(struct cpcss____ss *ss)
{   return cpcio_open_istream(cpcss_server_socket_get_client(ss), &cpcss_read_ss, &cpcss_close_ss);   }

struct cpcio____ostream *open_client_ostream(struct cpcss____ss *ss)
{   return cpcio_open_ostream(cpcss_server_socket_get_client(ss), &cpcss_write_ss, &cpcss_close_ss);   }

// open client streams
struct cpcio____istream *open_server_istream(struct cpcss____cs *cs)
{   return cpcio_open_istream(cpcss_client_socket_get_server(cs), &cpcss_read_ss, &cpcss_close_ss);   }

struct cpcio____ostream *open_server_ostream(struct cpcss____cs *cs)
{   return cpcio_open_ostream(cpcss_client_socket_get_server(cs), &cpcss_write_ss, &cpcss_close_ss);   }

int cpcss_read_ss(void *src,
                  char *arr,
                  size_t size)
{cpcss____sh *as = src;
#ifdef _WIN32
    return recv(*as, arr, size,0);
#elif defined __linux__
    return read(*as, arr, size);
#endif
}

int cpcss_write_ss(void *dest,
                     const char *arr,
                     size_t size)
{   cpcss____sh *ad = dest; return send(*ad, arr, size, 0);   }

int cpcss_close_ss(void *stream)
{cpcss____sh *aso = stream;
#ifdef _WIN32
    shutdown(*aso, SD_BOTH);
    return closesocket(*aso);
#elif defined __linux__
    shutdown(*aso, SHUT_RDWR);
    return close(*aso);
#endif
}

#endif //Included_header_only_sockstream_h
#endif //__cplusplus
