#ifndef __cplusplus
#ifndef Included_header_only_cpcss_socket_h
#define Included_header_only_cpcss_socket_h

#include<cpcss_socket.h>

// link the dll on msvc
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32")
#endif

// structs for sockets
struct __cpcss_sv_sk
  {   __sh _m_sv, _m_cl; __sa *_m_ar;   };

struct __cpcss_cl_sk
  {   __sh _m_sv; __sa *_m_ar;   };

// functions for opening sockets
struct __cpcss_sv_sk* cpcss_open_server(const char *__pt)
{__sh __ssk, __csk;
    __sa __ah, *__ahp;
#ifdef _WIN32
    ZeroMemory(&__ah, sizeof(__ah));
      __ah.ai_family = AF_UNSPEC, __ah.ai_socktype = SOCK_STREAM;
      __ah.ai_protocol = IPPROTO_TCP, __ah.ai_flags = AI_PASSIVE;
    int __ars=getaddrinfo(NULL, __pt, &__ah, &__ahp);
    if(__ars == 0)
    {   __ssk=socket(__ahp->ai_family, __ahp->ai_socktype, __ahp->ai_protocol);
        if(__ssk != INVALID_SOCKET)
        {   __ars = bind(__ssk, __ahp->ai_addr, __ahp->ai_addrlen);
            if(__ars == 0)
            {   __ars = listen(__ssk, SOMAXCONN);
                if(__ars == 0)
                {   __csk = accept(__ssk, NULL, NULL);
                    if(__csk != INVALID_SOCKET)
                    {   struct __cpcss_sv_sk *__sv=(struct __cpcss_sv_sk*)malloc(sizeof(struct __cpcss_sv_sk));
                        __sv->_m_ar = __ahp, __sv->_m_sv = __ssk, __sv->_m_cl = __csk;
                        return __sv;   } else
                    return NULL;   } else
                return NULL;   } else
            return NULL;   } else
        return NULL;   } else
    return NULL;
#elif defined __linux__
    __ssk=socket(AF_INET, SOCK_STREAM, 0);
    __ahp=&__ah;

    if (__ssk)
    {   __ah.sin_family = AF_INET;
        __ah.sin_addr.s_addr = INADDR_ANY;
        __ah.sin_port = htons(atoi(__pt));

        if(bind(__ssk, __ahp, sizeof(__ah)) >= 0 && listen(__ssk, 3) >= 0)
        {   socklen_t loas = sizeof(__ahp);
            __csk = accept(__ssk, __ahp, &loas);

            if(__csk >= 0)
            {   struct __cpcss_sv_sk *__sv=(struct __cpcss_sv_sk*)malloc(sizeof(struct __cpcss_sv_sk));
                __sv->_m_ar = __ahp, __sv->_m_sv = __ssk, __sv->_m_cl = __csk;
                return __sv;   } else
            return NULL;   } else
        return NULL;   } else
    return NULL;
#endif
}

struct __cpcss_cl_sk *cpcss_connect_client(const char *hn,const char *pt)
{__sh sv;
    __sa ad, *adp;
#ifdef _WIN32
    ZeroMemory(&ad,sizeof(__sa));
    ad.ai_family = AF_UNSPEC;
    ad.ai_socktype = SOCK_STREAM;
    ad.ai_protocol = IPPROTO_TCP;
    if(getaddrinfo(hn,pt,&ad,&adp)==0)
    {   sv = socket(adp->ai_family, adp->ai_socktype, adp->ai_protocol);
        if(sv != INVALID_SOCKET)
        {   int r = connect(sv, adp->ai_addr, adp->ai_addrlen);
            if(r == 0)
            {   struct __cpcss_cl_sk *csk = (struct __cpcss_cl_sk*)malloc(sizeof(struct __cpcss_cl_sk));
                csk->_m_sv = sv, csk->_m_ar = adp;
                return csk;   } else
            {   closesocket(sv); return NULL;   }   } else
        return NULL;   } else
    return NULL;
#elif defined __linux__
    sv = socket(AF_INET, SOCK_STREAM, 0);
    if(sv>=0)
    {   ad.sin_family = AF_INET;
        ad.sin_port=htons(atoi(pt));
        if(inet_pton(AF_INET, hn, &ad.sin_addr)>0 && connect(sv, &ad, sizeof(ad)) == 0)
        {   struct __cpcss_cl_sk *csk = (struct __cpcss_cl_sk*)malloc(sizeof(struct __cpcss_cl_sk));
            csk->_m_sv = sv, csk->_m_ar = adp;
            return csk;   } else
        return NULL;   } else
    return NULL;
#endif
}

// functions for getting members of structs
__sh *cpcss_client_socket_get_server(struct __cpcss_cl_sk *c)
{   return &c->_m_sv;   }
__sh *cpcss_server_socket_get_client(struct __cpcss_sv_sk *s)
{   return &s->_m_cl;   }

#endif //Included_header_only_cpcss_socket_h
#endif