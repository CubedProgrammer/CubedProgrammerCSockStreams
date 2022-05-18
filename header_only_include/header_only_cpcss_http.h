#ifndef __cplusplus
#ifndef Included_header_only_cpcss_http_h
#define Included_header_only_cpcss_http_h
#ifdef _WIN32
#include<winsock.h>
#else
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/ioctl.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cpcss_http.h>
#define P_CONST_A 375651977
#define P_CONST_B 149753843
#define P_CONST_C 987662329

int cpcss_init_http_request(pcpcss_http_req this, const char *url, uint16_t port)
{   size_t len = strlen(url);
    this->rru.req.port = port;
    this->rru.req.meth = CPCSS_GET;
    this->body = NULL;
    this->hcnt = 0;
    this->hbuckets = 60;
    this->headers = malloc(sizeof(char *) * this->hbuckets);
    int succ = 0;
    if(this->headers != NULL)
    {   for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
        	*it = NULL;
        this->rru.req.requrl = malloc(1 + len);
        if(this->rru.req.requrl != NULL)
        {   strcpy(this->rru.req.requrl, url);
            char *slash = strchr(this->rru.req.requrl, '/');
            if(slash != NULL)
            {   *slash = '\0';
                if(cpcss_set_header(this, "host", this->rru.req.requrl))
                    succ = -1; else
                *slash = '/';   } else
            return cpcss_set_header(this, "host", "/");   } else
        return-1;   } else
    succ = -1;
    return succ;   }

int cpcss_init_http_response(pcpcss_http_req this, cpcss_res_code_t res, char *body)
{   this->hcnt = 0;
    this->hbuckets = 60;
    this->headers = malloc(sizeof(char *) * this->hbuckets);
    if(res >= 100 && res <= 599)
        this->rru.res = res;
    else
        this->rru.res = 200;
    int succ = 0;
    if(this->headers != NULL)
    {   for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
            *it = NULL;
        succ = -1;
        this->body = body;   }
    else
        this->body = NULL;
    return succ;   }

int cpcss_http____insens_strcmp(const char *x, const char *y)
{   char chx, chy;
    int e = 1;
    for(const char *xit = x, *yit = y; e && *xit != '\0' && *yit != '\0'; ++xit, ++yit)
    {   chx = *xit, chy = *yit;
        if(chx >= 'A' && chx <= 'Z')
            chx += 32;
        if(chy >= 'A' && chy <= 'Z')
            chy += 32;
        e = chx == chy;   }
    return e;   }

uint64_t cpcss_http____hash_key(const char *key)
{   uint64_t hsh;
    uint64_t x = 0, y = 0, z = 0;
    char ch;
    for(const char *it = key; *it != '\0'; ++it)
    {   ch = *it;
        if(ch >= 'A' && ch <= 'Z')
            ch += 32;
        x *= P_CONST_A;
        x += P_CONST_B + ch;
        y *= P_CONST_B;
        y += P_CONST_C + ch;
        z *= P_CONST_C;
        z += P_CONST_A + ch;   }
    hsh = x + y + z;
    return hsh;   }

char **cpcss_http____get_bucket(cpcpcss_http_req this, const char *key)
{   uint64_t hsh = cpcss_http____hash_key(key);
    hsh %= this->hbuckets;
    char **it;
    char *tmp = NULL;
    for(it = this->headers + hsh; *it != NULL; ++it)
    {   if(cpcss_http____insens_strcmp(*it, key))
        {   tmp = *it;
            *it = NULL;
            --it;   } else if(it + 1 == this->headers + this->hbuckets)
        it = this->headers - 1;   }
    *it = tmp;
    return it;   }

int cpcss_http____check_resize(pcpcss_http_req this)
{   size_t thresh = this->hbuckets * 0.6;
	int succ = 0;
    if(this->hcnt >= thresh)
    {   size_t nsz = this->hbuckets * 3 / 2;
    	uint64_t hsh;
        char **new = malloc(nsz * sizeof(*new));
        if(new != NULL)
        {   memset(new, 0, nsz * sizeof(*new));
            for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
            {   if(*it != NULL)
        	    {   hsh = cpcss_http____hash_key(*it);
        	        hsh %= nsz;
        	        for(; new[hsh] != NULL; ++hsh, hsh %= nsz);
        	        new[hsh] = *it;   }   }
            free(this->headers);
            this->headers = new;
            this->hbuckets = nsz;   } else
        succ = -1;   }
    return succ;   }

int cpcss_http____req_meth_str(char *str, cpcss_req_method_t meth)
{   int succ = 0;
    switch(meth)
    {   case CPCSS_GET:
            strcpy(str, "GET");
            break;
        case CPCSS_PUT:
            strcpy(str, "PUT");
            break;
        case CPCSS_HEAD:
            strcpy(str, "HEAD");
            break;
        case CPCSS_POST:
            strcpy(str, "POST");
            break;
        case CPCSS_PATCH:
            strcpy(str, "PATCH");
            break;
        case CPCSS_TRACE:
            strcpy(str, "TRACE");
            break;
        case CPCSS_DELETE:
            strcpy(str, "DELETE");
            break;
        case CPCSS_OPTIONS:
            strcpy(str, "OPTIONS");
            break;
        case CPCSS_CONNECT:
            strcpy(str, "CONNECT");
            break;
        default:
        	succ = -1;   }
    return succ;   }

int cpcss_set_req_method(pcpcss_http_req this, cpcss_req_method_t meth)
{   int succ = meth > CPCSS_CONNECT || meth < CPCSS_GET;
    if(succ)
        succ *= -1;
    else
        this->rru.req.meth = meth;
    return succ;   }

int cpcss_set_header(pcpcss_http_req this, const char *key, const char *val)
{   if(this->headers != NULL)
    {   cpcss_http____check_resize(this);
        char **it = cpcss_http____get_bucket(this, key);
        size_t klen = strlen(key), vlen = strlen(val);
        if(*it != NULL)
        	free(*it);
        *it = malloc(klen + vlen + 3);
        if(*it != NULL)
        {   strcpy(*it, key);
            strcpy(*it + klen + 1, val);
            *(*it + klen + vlen + 2) = '\0';
            ++this->hcnt;
            return 0;   } else
        return-1;   } else
    return-1;   }

int cpcss_add_header(pcpcss_http_req this, const char *key, const char *val)
{   if(this->headers != NULL)
    {   char **it = cpcss_http____get_bucket(this, key);
        size_t klen = strlen(key), vlen;
        if(*it != NULL)
        {   vlen = strlen(*it + klen + 1);
            char *tmp = calloc(klen + vlen + strlen(val) + 5, sizeof(char));
            if(tmp != NULL)
            {   strcpy(tmp, *it);
                strcpy(tmp + klen + 1, *it + klen + 1);
                strcpy(tmp + klen + vlen + 1, ", ");
                strcpy(tmp + klen + vlen + 3, val);
                free(*it);
                *it = tmp;
                return 0;   } else
            return-1;   } else
        {   cpcss_http____check_resize(this);
            it = cpcss_http____get_bucket(this, key);
            vlen = strlen(val);
            *it = malloc(klen + vlen + 3);
            if(*it != NULL)
            {   strcpy(*it, key);
                strcpy(*it + klen + 1, val);
                *(*it + klen + vlen + 2) = '\0';
                ++this->hcnt;
                return 0;   } else
            return-1;   }   } else
	return-1;   }

int cpcss_erase_header(pcpcss_http_req this, const char *key)
{   if(this->headers != NULL)
    {   char **bucket = cpcss_http____get_bucket(this, key);
        if(*bucket != NULL)
        {   free(*bucket);
            *bucket = NULL;
            --this->hcnt;
            return 0;   } else
        return-1;   } else
    return-1;   }

const char *cpcss_get_header(cpcpcss_http_req this, const char *key)
{   char **bucket = cpcss_http____get_bucket(this, key);
    const char *val = *bucket;
    if(val != NULL)
    	val += strlen(val) + 1;
    return val;   }

int cpcss_make_request(cpcpcss_http_req this, cpcss_client_sock *cs, pcpcss_http_req res)
{   int succ = 0;
    size_t reqsz = cpcss_request_size(this);
    char *reqdat = malloc(reqsz);
    if(reqdat != NULL)
    {   cpcss_request_str(reqdat, this);
        struct addrinfo *addrls;
        getaddrinfo(cpcss_get_header(this, "host"), NULL, NULL, &addrls);
        char port[7];
        sprintf(port, "%u", this->rru.req.port);
        const char *ipstr;
        struct sockaddr_in *addrin;
        for(struct addrinfo *ainode = addrls; ainode != NULL; ainode = ainode->ai_next)
        {   addrin = (struct sockaddr_in *)ainode->ai_addr;
            ipstr = inet_ntoa(addrin->sin_addr);
            *cs = cpcss_connect_client(ipstr, port);
            if(*cs != NULL)
                goto fini;   }
        fini:
        freeaddrinfo(addrls);
		if(*cs != NULL)
        {   cpcss____sh sock = *cpcss_client_socket_get_server(*cs);
            if(send(sock, reqdat, reqsz, 0) < reqsz)
            {   succ = CPCSS_REQ_CONNECTION_ERROR;
                cpcss_discon_client(*cs);   } else
            {
                int ressz;
#ifdef _WIN32
                succ = ioctlsocket(sock, FIONREAD, &ressz);
#else
                succ = ioctl(sock, FIONREAD, &ressz);
#endif
                if(succ != -1)
                {   char *resdat = malloc(1 + ressz);
                    if(resdat != NULL)
                    {
                        succ
#ifdef _WIN32
                         = recv(sock, resdat, ressz, 0);
#else
                         = read(sock, resdat, ressz);
#endif
                        resdat[succ] = '\0';
					} else
                    succ = CPCSS_REQ_MEMORY_ERROR;   } else
                succ = CPCSS_REQ_CONNECTION_ERROR;   }   } else
        succ = CPCSS_REQ_CONNECTION_ERROR;   } else
    succ = CPCSS_REQ_MEMORY_ERROR;
    return succ;   }

size_t cpcss____reqsz(cpcpcss_http_req this)
{   size_t cnt = 0, klen;
    for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
    {   if(*it != NULL)
        {   klen = strlen(*it);
            cnt += klen + 2;
            cnt += strlen(*it + klen + 1) + 2;   }   }
	cnt += 2;
    if(this->body != NULL)
        cnt += strlen(this->body);
    return cnt;   }

void cpcss____req_str(char *str, cpcpcss_http_req this)
{   size_t klen;
    for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
    {   if(*it != NULL)
        {   klen = strlen(*it);
            strcpy(str, *it);
            str += klen;
            strcpy(str, ": ");
            str += 2;
            strcpy(str, *it + klen + 1);
            str += strlen(*it + klen + 1);
            *str++ = '\r';
            *str++ = '\n';   }   }
    *str++ = '\r';
    *str++ = '\n';
    *str = '\0';
    if(this->body != NULL)
        strcpy(str, this->body);   }

size_t cpcss_request_size(cpcpcss_http_req this)
{   char reqmeth[12];
    cpcss_http____req_meth_str(reqmeth, this->rru.req.meth);
    size_t cnt = strlen(reqmeth) + 1;
    char *pth = strchr(this->rru.req.requrl, '/');
    if(pth != NULL)
        cnt += strlen(pth) + 11;
    else
        cnt += 12;
    cnt += cpcss____reqsz(this);
    return cnt;   }

void cpcss_request_str(char *str, cpcpcss_http_req this)
{   char *strptr = str;
    cpcss_http____req_meth_str(strptr, this->rru.req.meth);
    strptr += strlen(strptr);
    *strptr = ' ';
    ++strptr;
    char *pth = strchr(this->rru.req.requrl, '/');
    if(pth != NULL)
    {   strcpy(strptr, pth);
        strptr += strlen(strptr);   } else
    *strptr++ = '/';
    *strptr++ = ' ';
    strcpy(strptr, "HTTP/1.1\r\n");
    strptr += strlen(strptr);
    cpcss____req_str(strptr, this);   }

size_t cpcss_response_size(cpcpcss_http_req this)
{   size_t cnt = 15 + cpcss____reqsz(this);
    return cnt;   }

void cpcss_response_str(char *str, cpcpcss_http_req this)
{   char *strptr = str;
    strcpy(strptr, "HTTP/1.1 ");
    strptr += 9;
    cpcss_res_code_t res = this->rru.res;
    *strptr++ = res / 100 + '0';
    *strptr++ = res / 10 % 10 + '0';
    *strptr++ = res % 10 + '0';
    strcpy(strptr, " \r\n");
    strptr += 3;
    cpcss____req_str(strptr, this);   }

int cpcss____parse_http_req(const char *str, pcpcss_http_req res)
{   int succ = 0;
    const char *ptr;
    size_t colon, crlf;
    size_t kind, vind;
    char kstr[2601], vstr[2601];
    for(; succ == 0; str += crlf)
    {   ptr = strstr(str, "\r\n");
        if(ptr == NULL)
            goto fail;
        crlf = ptr - str;
        if(crlf == 0)
            goto next;
        ptr = strchr(str, ':');
        if(ptr == NULL)
            goto fail;
        colon = ptr - str;
        if(colon > crlf)
            goto fail;
        if(*str == ' ')
            goto fail;
        kind = colon;
        while(str[--kind] == ' ');
        while(str[++colon] == ' ');
        if(colon == crlf)
            goto fail;
        vind = crlf;
        while(str[--vind] == ' ');
        if(vind - colon + 1 < sizeof(vstr) && kind + 1 < sizeof(kstr))
        {   strncpy(kstr, str, kind + 1);
            strncpy(vstr, str + colon, vind - colon + 1);
            kstr[kind + 1] = '\0';
            vstr[vind - colon + 1] = '\0';
            succ = cpcss_set_header(res, kstr, vstr);   } else
        fail:succ = -1;   }
    next:
    return succ;   }

int cpcss_parse_response(const char *str, pcpcss_http_req res)
{   int succ = 0;
    res->rru.res = 0;
    const char *numstr = strchr(str, ' ');
    if(numstr == NULL)
        succ = -1;
    else
    {   ++numstr;
        for(; *numstr != ' ' && res->rru.res < 600; ++numstr)
        {   if(*numstr >= '0' && *numstr <= '9')
            {   res->rru.res *= 10;
                res->rru.res += *numstr - '0';   } else
            res->rru.res = 700;   }
        if(res->rru.res > 99 && res->rru.res < 600)
        {   numstr = strchr(numstr, '\n');
            if(numstr == NULL)
                succ = -1; else
            succ = cpcss____parse_http_req(numstr + 1, res);   } else
        succ = -1;   }
    return succ;   }

void cpcss_free_request(pcpcss_http_req this)
{   if(this->rru.req.requrl != NULL)
        free(this->rru.req.requrl);
	if(this->headers != NULL)
	{   for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
        {   if(*it != NULL)
                free(*it);   }
        free(this->headers);   }
    if(this->body != NULL)
        free(this->body);   }

#endif
#endif
