#ifndef __cplusplus
#ifndef Included_header_only_cpcss_socket_h
#define Included_header_only_cpcss_socket_h

#include<cpcss_socket.h>
#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

// link the dll on msvc
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32")
#endif

// structs for sockets
struct cpcss_socket_impl
{   cpcss____sh _m_sv;
    cpcss____sa _m_ar;   };

// functions for opening sockets
struct cpcss_socket_impl* cpcss_open_server(uint16_t port)
{   return cpcss_open_server_ex(port, 0);   }

struct cpcss_socket_impl* cpcss_open_server_ex(uint16_t port,int v6)
{
    cpcss____sh ssk;
    cpcss____sa ah, *ahp;
    int socktype = v6 ? AF_INET6 : AF_INET;
    ssk=socket(socktype, SOCK_STREAM, 0);
    ahp=&ah;
    if(ssk)
    {   ah.sin_family = socktype;
        ah.sin_addr.s_addr = INADDR_ANY;
        ah.sin_port = htons(port);
        if(bind(ssk, (struct sockaddr *)ahp, sizeof(ah)) == 0 && listen(ssk, 3) >= 0)
        {   struct cpcss_socket_impl* sv = malloc(sizeof(*sv));
            sv->_m_ar = ah, sv->_m_sv = ssk;
            return sv;   } else
        return NULL;   } else
    return NULL;   }

struct cpcss_socket_impl* cpcss_accept_client(struct cpcss_socket_impl *sv)
{   cpcss____sh ssk = sv->_m_sv, csk;
    socklen_t loas = sizeof(sv->_m_ar);
    csk = accept(ssk, (struct sockaddr *)&sv->_m_ar, &loas);
#ifdef _WIN32
    if(csk != INVALID_SOCKET)
#else
    if(csk >= 0)
#endif
    {   struct cpcss_socket_impl *cli=malloc(sizeof(*cli));
        cli->_m_ar = sv->_m_ar, cli->_m_sv = csk;
        return cli;   } else
    return NULL;   }

struct cpcss_socket_impl *cpcss_connect_client(const char *host,uint16_t port)
{   return cpcss_connect_client_ex(host, port, 0);   }

struct cpcss_socket_impl* cpcss_connect_client_ex(const char *host,uint16_t port,int v6)
{   cpcss____sh sv;
    cpcss____sa ad, *adp = &ad;
    sv = socket(AF_INET, SOCK_STREAM, 0);
    if(sv>=0)
    {   ad.sin_family = AF_INET;
        ad.sin_port=htons(port);
        if(inet_pton(AF_INET, host, &ad.sin_addr)>0 && connect(sv, (struct sockaddr *)&ad, sizeof(ad)) == 0)
        {   struct cpcss_socket_impl *csk = malloc(sizeof(*csk));
            csk->_m_sv = sv, csk->_m_ar = ad;
            return csk;   } else
        return(close(sv), NULL);   } else
    return NULL;   }

int cpcss_close_server(struct cpcss_socket_impl *sv)
{   cpcss____sh aso = sv->_m_sv;
#ifdef _WIN32
	return closesocket
#else
    return close
#endif
    (aso);   }

int cpcss_discon_client(struct cpcss_socket_impl *cs)
{   cpcss____sh aso = cs->_m_sv;
#ifdef _WIN32
	return closesocket
#else
    return close
#endif
    (aso);   }

cpcss____sh *cpcss_get_raw_socket(struct cpcss_socket_impl *socket)
{   return&socket->_m_sv;   }
// functions for getting members of structs
cpcss____sh *cpcss_client_socket_get_server(struct cpcss_socket_impl *c)
{   return &c->_m_sv;   }
cpcss____sh *cpcss_server_socket_get_server(struct cpcss_socket_impl *s)
{   return &s->_m_sv;   }

#endif //Included_header_only_cpcss_socket_h
#endif
