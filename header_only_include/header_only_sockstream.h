#ifndef __cplusplus
#ifndef Included_header_only_sockstream_h
#define Included_header_only_sockstream_h

#include <sockstream.h>

// open client streams
struct __istream *open_client_istream(struct __cpcss_sv_sk *ss)
{   return openis(cpcss_server_socket_get_client(ss), &__cpcss_read_ss, &__cpcss_close_ss);   }

struct __ostream *open_client_ostream(struct __cpcss_sv_sk *ss)
{   return openos(cpcss_server_socket_get_client(ss), &__cpcss_write_ss, &__cpcss_close_ss);   }

// open client streams
struct __istream *open_server_istream(struct __cpcss_cl_sk *cs)
{   return openis(cpcss_client_socket_get_server(cs), &__cpcss_read_ss, &__cpcss_close_ss);   }

struct __ostream *open_server_ostream(struct __cpcss_cl_sk *cs)
{   return openos(cpcss_client_socket_get_server(cs), &__cpcss_write_ss, &__cpcss_close_ss);   }

int __cpcss_read_ss(void *src,
                  char *arr,
                  size_t size)
{__sh *as = src;
#ifdef _WIN32
    return recv(*as, arr, size,0);
#elif defined __linux__
    return read(*as, arr, size);
#endif
}

int __cpcss_write_ss(void *dest,
                     const char *arr,
                     size_t size)
{   __sh *ad = dest; return send(*ad, arr, size, 0);   }

int __cpcss_close_ss(void *stream)
{__sh *aso = stream;
#ifdef _WIN32
    shutdown(*aso, SD_BOTH);
    return closesocket(*aso);
#elif __linux__
    shutdown(*aso, SHUT_RDWR);
    return close(*aso);
#endif
}

#endif //Included_header_only_sockstream_h
#endif //__cplusplus