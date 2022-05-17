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
struct cpcss____ss
  {   cpcss____sh _m_sv, _m_cl; cpcss____sa *_m_ar;   };

struct cpcss____cs
  {   cpcss____sh _m_sv; cpcss____sa *_m_ar;   };

// functions for opening sockets
struct cpcss____ss* cpcss_open_server(const char *__pt)
{   cpcss____sh ssk, csk;
    cpcss____sa ah, *ahp;
#ifdef _WIN32
    ZeroMemory(&ah, sizeof(ah));
      ah.ai_family = AF_UNSPEC, ah.ai_socktype = SOCK_STREAM;
      ah.ai_protocol = IPPROTO_TCP, ah.ai_flags = AI_PASSIVE;
    int ars=getaddrinfo(NULL, __pt, &ah, &ahp);
    if(ars == 0)
    {   ssk=socket(ahp->ai_family, ahp->ai_socktype, ahp->ai_protocol);
        if(ssk != INVALID_SOCKET)
        {   ars = bind(ssk, ahp->ai_addr, ahp->ai_addrlen);
            if(ars == 0)
            {   ars = listen(ssk, SOMAXCONN);
                if(ars == 0)
                {   csk = accept(ssk, NULL, NULL);
                    if(csk != INVALID_SOCKET)
                    {   struct cpcss____ss *sv=(struct cpcss____ss*)malloc(sizeof(struct cpcss____ss));
                        sv->_m_ar = ahp, sv->_m_sv = ssk, sv->_m_cl = csk;
                        return sv;   } else
                    return NULL;   } else
                return NULL;   } else
            return NULL;   } else
        return NULL;   } else
    return NULL;
#else
    ssk=socket(AF_INET, SOCK_STREAM, 0);
    ahp=&ah;

    if (ssk)
    {   ah.sin_family = AF_INET;
        ah.sin_addr.s_addr = INADDR_ANY;
        ah.sin_port = htons(atoi(__pt));

        if(bind(ssk, (struct sockaddr *)ahp, sizeof(ah)) >= 0 && listen(ssk, 3) >= 0)
        {   socklen_t loas = sizeof(ahp);
            csk = accept(ssk, (struct sockaddr *)ahp, &loas);

            if(csk >= 0)
            {   struct cpcss____ss *sv=(struct cpcss____ss*)malloc(sizeof(struct cpcss____ss));
                sv->_m_ar = ahp, sv->_m_sv = ssk, sv->_m_cl = csk;
                return sv;   } else
            return NULL;   } else
        return NULL;   } else
    return NULL;
#endif
}

struct cpcss____ss* cpcss_accept_client(struct cpcss____ss *sv)
{
    cpcss____sh ssk = sv->_m_sv, csk;
#ifdef _WIN32
    csk = accept(ssk, NULL, NULL);
    if(csk != INVALID_SOCKET)
    {   struct cpcss____ss *nsv=(struct cpcss____ss*)malloc(sizeof(struct cpcss____ss));
        nsv->_m_ar = sv->_m_ar, nsv->_m_sv = ssk, nsv->_m_cl = csk;
        return nsv;   } else
    return NULL;
#else
    socklen_t loas = sizeof(sv->_m_ar);
    csk = accept(ssk, (struct sockaddr *)sv->_m_ar, &loas);

    if(csk >= 0)
    {   struct cpcss____ss *nsv=(struct cpcss____ss*)malloc(sizeof(struct cpcss____ss));
        nsv->_m_ar = sv->_m_ar, nsv->_m_sv = ssk, nsv->_m_cl = csk;
        return nsv;   } else
    return NULL;
#endif
}

struct cpcss____cs *cpcss_connect_client(const char *hn,const char *pt)
{cpcss____sh sv;
    cpcss____sa ad, *adp = &ad;
#ifdef _WIN32
    ZeroMemory(&ad,sizeof(cpcss____sa));
    ad.ai_family = AF_UNSPEC;
    ad.ai_socktype = SOCK_STREAM;
    ad.ai_protocol = IPPROTO_TCP;
    if(getaddrinfo(hn,pt,&ad,&adp)==0)
    {   sv = socket(adp->ai_family, adp->ai_socktype, adp->ai_protocol);
        if(sv != INVALID_SOCKET)
        {   int r = connect(sv, adp->ai_addr, adp->ai_addrlen);
            if(r == 0)
            {   struct cpcss____ss *csk = (struct cpcss____ss*)malloc(sizeof(struct cpcss____ss));
                csk->_m_sv = sv, csk->_m_ar = adp;
                return csk;   } else
            {   closesocket(sv); return NULL;   }   } else
        return NULL;   } else
    return NULL;
#else
    sv = socket(AF_INET, SOCK_STREAM, 0);
    if(sv>=0)
    {   ad.sin_family = AF_INET;
        ad.sin_port=htons(atoi(pt));
        if(inet_pton(AF_INET, hn, &ad.sin_addr)>0 && connect(sv, (struct sockaddr *)&ad, sizeof(ad)) == 0)
        {   struct cpcss____cs *csk = (struct cpcss____cs*)malloc(sizeof(struct cpcss____ss));
            csk->_m_sv = sv, csk->_m_ar = adp;
            return csk;   } else
        return NULL;   } else
    return NULL;
#endif
}

int cpcss_close_server(struct cpcss____ss *sv)
{cpcss____sh aso = sv->_m_cl;
#ifdef _WIN32
	return closesocket(aso);
#else
    return close(aso);
#endif
}

int cpcss_discon_client(struct cpcss____cs *cs)
{cpcss____sh aso = cs->_m_sv;
#ifdef _WIN32
	return closesocket(aso);
#else
    return close(aso);
#endif
}

// functions for getting members of structs
cpcss____sh *cpcss_client_socket_get_server(struct cpcss____cs *c)
{   return &c->_m_sv;   }
cpcss____sh *cpcss_server_socket_get_client(struct cpcss____ss *s)
{   return &s->_m_cl;   }

#endif //Included_header_only_cpcss_socket_h
#endif
