#ifndef __cplusplus
#ifndef Included_header_only_cpcss_http_h
#define Included_header_only_cpcss_http_h
#ifdef _WIN32
//#include<winsock.h>
#else
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/select.h>
#include<sys/ioctl.h>
#endif
#include<ctype.h>
#include<limits.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cpcss_http.h>
#define P_CONST_A 375651977
#define P_CONST_B 149753843
#define P_CONST_C 987662329
#define PARSE_CHUNK_LENGTH 32768

int cpcss_init_http_request(pcpcss_http_req this, const char *url, uint16_t port)
{   int succ = cpcss_init_http_headers(this);
    if(succ == 0)
    {   size_t len = strlen(url);
        this->rru.req.requrl = malloc(1 + len);
        this->rru.req.port = port;
        this->rru.req.meth = CPCSS_GET;
        this->body = NULL;
        if(this->rru.req.requrl != NULL)
        {   strcpy(this->rru.req.requrl, url);
            char *slash = strchr(this->rru.req.requrl, '/');
            if(slash != NULL)
            {   *slash = '\0';
                if(cpcss_set_header(this, "host", this->rru.req.requrl))
                    succ = -1; else
                *slash = '/';   } else
            succ = cpcss_set_header(this, "host", this->rru.req.requrl);   } else
        succ = -1;   }
    return succ;   }

int cpcss_init_http_response(pcpcss_http_req this, cpcss_res_code_t res, char *body)
{   int succ = cpcss_init_http_headers(this);
    if(succ == 0)
    {   if(res >= 100 && res <= 599)
            this->rru.res = res; else
        this->rru.res = 200;
        this->body = body;   } else
    this->body = NULL;
    return succ;   }

int cpcss_init_http_headers(pcpcss_http_req this)
{   this->hcnt = 0;
    this->hbuckets = 60;
    this->headers = malloc(sizeof(char*) * this->hbuckets);
    this->body = NULL;
    if(this->headers != NULL)
    {   for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
            *it = NULL;
        return 0;   } else
    return -1;   }

int cpcss_http_cpy(pcpcss_http_req dest, cpcpcss_http_req src)
{   int succ = 0;
    dest->rru = src->rru;
    dest->hbuckets = src->hbuckets;
    dest->hcnt = src->hcnt;
    size_t bodylen;
    if(src->body != NULL)
    {
        bodylen = strlen(src->body);
        dest->body = malloc(bodylen + 1);
    }
    else
    	dest->body = NULL;
    if(src->body != NULL && dest->body == NULL)
        succ = -1;
    else
    {   if(src->body != NULL)
        strcpy(dest->body, src->body);
        dest->headers = malloc(dest->hbuckets * sizeof(*dest->headers));
        if(dest->headers == NULL)
            succ = -1;
        else
        {   size_t klen, vlen;
            for(size_t i = 0; i < dest->hbuckets; ++i)
			{   if(src->headers[i] != NULL)
                {   klen = strlen(src->headers[i]);
                    vlen = strlen(src->headers[i] + klen + 1);
                    dest->headers[i] = malloc(klen + vlen + 2);
                    if(dest->headers[i] == NULL)
                        succ = -1;
                    else
                        memcpy(dest->headers[i], src->headers[i], klen + vlen + 2);   }
                else
                    dest->headers[i] = NULL;   }   }   }
    return succ;   }

int cpcss_http____insens_strcmp(const char *x, const char *y)
{   char chx, chy;
    int e = 1;
    const char *xit, *yit;
    for(xit = x, yit = y; e && *xit != '\0' && *yit != '\0'; ++xit, ++yit)
    {   chx = *xit, chy = *yit;
        if(chx >= 'A' && chx <= 'Z')
            chx += 32;
        if(chy >= 'A' && chy <= 'Z')
            chy += 32;
        e = chx == chy;   }
    return e && *xit == *yit;   }

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

int cpcss_http_req_meth_num(const char *str, cpcss_req_method_t *meth)
{   int succ = 0;
    if(strcmp(str, "GET") == 0)
    *meth = CPCSS_GET; else if(strcmp(str, "PUT") == 0)
    *meth = CPCSS_PUT; else if(strcmp(str, "HEAD") == 0)
    *meth = CPCSS_HEAD; else if(strcmp(str, "POST") == 0)
    *meth = CPCSS_POST; else if(strcmp(str, "PATCH") == 0)
    *meth = CPCSS_PATCH; else if(strcmp(str, "TRACE") == 0)
    *meth = CPCSS_TRACE; else if(strcmp(str, "DELETE") == 0)
    *meth = CPCSS_DELETE; else if(strcmp(str, "CONNECT") == 0)
    *meth = CPCSS_CONNECT; else if(strcmp(str, "OPTIONS") == 0)
    *meth = CPCSS_OPTIONS; else
    succ = -1;
    return succ;   }

int cpcss_http_req_meth_str(char *str, cpcss_req_method_t meth)
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
        case CPCSS_CONNECT:
            strcpy(str, "CONNECT");
            break;
        case CPCSS_OPTIONS:
            strcpy(str, "OPTIONS");
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

int cpcss_make_request_sync(cpcpcss_http_req this, pcpcss_http_req res, cpcio_istream is, cpcio_ostream os)
{   int succ = cpcss_send_request(this, os);
    if(succ == 0)
    {   succ = cpcss_parse_response(is, res);   }
    return succ;   }

int cpcss_send_request(cpcpcss_http_req this, cpcio_ostream os)
{   int succ = 0;
    size_t reqsz = cpcss_request_size(this);
    char *reqdat = malloc(reqsz);
    if(reqdat != NULL)
    {   cpcss_request_str(reqdat, this);
        cpcio_wr(os, reqdat, reqsz);
        cpcio_flush_os(os);
        free(reqdat);   } else
    succ = CPCSS_REQ_MEMORY_ERROR;
    return succ;   }

int cpcss_connect_http(cpcpcss_http_req this, cpcss_socket *cs)
{   int succ = 0;
    const char *host = cpcss_get_header(this, "host");
    struct in_addr ia;
    int valid = inet_aton(host, &ia);
    if(valid)
	{   *cs = cpcss_connect_client(host, this->rru.req.port);   } else
    {   struct addrinfo *addrls;
        int v = getaddrinfo(host, NULL, NULL, &addrls);
        if(v == 0)
        {   const char *ipstr;
            struct sockaddr_in *addrin;
            for(struct addrinfo *ainode = addrls; ainode != NULL; ainode = ainode->ai_next)
            {   addrin = (struct sockaddr_in *)ainode->ai_addr;
                ipstr = inet_ntoa(addrin->sin_addr);
                *cs = cpcss_connect_client(ipstr, this->rru.req.port);
                if(*cs != NULL)
                    goto fini;   }
            succ = CPCSS_REQ_CONNECTION_ERROR;
            fini:
            freeaddrinfo(addrls);   } else
        succ = CPCSS_REQ_DOMAIN_ERROR;   }
    return succ;   }

int cpcss_parse_request(cpcio_istream is, pcpcss_http_req req)
{   return cpcss_parse_request_ex(is, req, LONG_MAX, LONG_MAX, NULL);   }

int cpcss_parse_request_ex(cpcio_istream is, pcpcss_http_req req, long tlimit, size_t climit, cpcpcss_http_req filter)
{   int succ = 0;
    const char *delim = cpcio_get_delim(is);
    char lastdelim[121];
    strcpy(lastdelim, delim);
    cpcio_use_delim(is, "\n");
    char *str = cpcio_gtoken_is(is);
    cpcio_use_delim(is, lastdelim);
    if(str != NULL)
    {   char *space = strchr(str, ' ');
        if(space != NULL)
        {   *space = '\0';
            succ = cpcss_init_http_headers(req);
            if(succ == 0)
            {   succ = cpcss_http_req_meth_num(str, &req->rru.req.meth);
                if(succ == 0)
                {   char *nextspace = strchr(++space, ' ');
                    if(nextspace != NULL)
                    {   req->rru.req.requrl = malloc(nextspace - space + 1);
                        if(req->rru.req.requrl != NULL)
                        {   memcpy(req->rru.req.requrl, space, nextspace - space);
                            req->rru.req.requrl[nextspace - space] = '\0';
                            succ = cpcss_parse_http_stream_ex(is, req, tlimit, climit, filter);   } else
                        succ = -1;   } else
                    succ = -1;   }   }   } else
        succ = -1;
        free(str);   } else
    succ = -1;
    return succ;   }

int cpcss_parse_response(cpcio_istream is, pcpcss_http_req res)
{   int succ = 0;
    uint16_t resnum = 0;
    char *numstr = cpcio_gtoken_is(is);
    free(numstr);
    numstr = cpcio_gtoken_is(is);
    if(numstr == NULL)
        succ = -1;
    else
    {   char *it = numstr;
        for(; *it != '\0' && resnum < 600; ++it)
        {   if(*it >= '0' && *it <= '9')
            {   resnum *= 10;
                resnum += *it - '0';   } else
            resnum = 700;   }
        if(resnum > 99 && resnum < 600)
        {   int x = cpcio_getc_is(is), y = cpcio_getc_is(is);
            while(!cpcio_eof_is(is) && (x != '\r' || y != '\n'))
            {   x = y;
                y = cpcio_getc_is(is);   }
            if(cpcio_eof_is(is))
                succ = -1; else
            {   cpcss_init_http_response(res, resnum, NULL);
                succ = cpcss_parse_http_stream(is, res);   }   } else
        succ = -1;
        free(numstr);   }
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
    cpcss_http_req_meth_str(reqmeth, this->rru.req.meth);
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
    cpcss_http_req_meth_str(strptr, this->rru.req.meth);
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

int cpcss_init_partial_parser(struct cpcss_partial_parse_data *dat, unsigned field_limit)
{   dat->cnt = 0;
    dat->field_size_limit = field_limit;
    dat->isvalue = 0;
    dat->ignore = 0;
    dat->last_linefeed = 0;
    dat->body = 0;
    return(dat->dat = malloc(dat->field_size_limit + 1)) == NULL;   }

void cpcss_free_partial_parser(struct cpcss_partial_parse_data *dat)
{   free(dat->dat);   }

void cpcss_partial_copy(struct cpcss_partial_parse_data *dat, char tocopy, const char *first, const char *last, pcpcss_http_req out)
{   size_t copycnt = last - first;
    if(dat->cnt + copycnt < dat->field_size_limit)
    {   memcpy(dat->dat + dat->cnt, first, copycnt);
        if(tocopy >> 1 == 1)
            dat->dat[copycnt + dat->cnt] = '\0';
        dat->cnt += copycnt + (tocopy >> 1);   } else
    {   dat->ignore = 1;
        dat->isvalue = 1;   }
    if(tocopy == 3 && !dat->ignore)
    {   char *key = dat->dat;
        char *val = dat->dat;
        for(; *val != '\0'; ++val);
        for(++val; val != dat->dat + dat->cnt && isspace(*val); ++val);
        for(--dat->cnt; dat->dat[dat->cnt] != '\0' && isspace(dat->dat[dat->cnt]); --dat->cnt);
        dat->dat[++dat->cnt] = '\0';
        cpcss_set_header(out, key, val);
        dat->cnt = 0;   }   }

unsigned cpcss_partial_parse_header(struct cpcss_partial_parse_data *dat, const char *ptr, unsigned len, pcpcss_http_req out)
{   static const char searchchars[] = ":\r";
    unsigned remaining = 0;
    const char *cpbegin = ptr, *cpend = ptr;
    const char*old, *end = ptr + len;
    char tofind;
    char tocopy = 0;
    if(!dat->body)
    {   if(dat->last_linefeed)
        {   if(dat->last_linefeed == 3)
            {   dat->body = ptr[0] == '\n';
                dat->isvalue = !dat->body;
                dat->last_linefeed = 0;   }
            else if(len + dat->last_linefeed >= 2)
            {   const char *checkptr = ptr + (2 - dat->last_linefeed);
                if(isspace(*checkptr))
                {   if(*checkptr == '\r')
                    {   if(checkptr + 1 == end)
                            dat->last_linefeed = 3; else
                        dat->body = checkptr[1] == '\n';   } else
                    {   len -= checkptr - ptr + 1;
                        ptr = checkptr + 1;   }   } else
                {   dat->last_linefeed = 0;
                    dat->isvalue = 0;   }   } else
            dat->last_linefeed += len;   }   }
    if(!dat->body)
    {   for(const char*separator = ptr; separator != end; separator += (separator != end) * (2 - dat->isvalue))
        {   tofind = searchchars[dat->isvalue];
            separator = memchr(old = separator, tofind, end - separator);
            separator = separator == NULL ? end : separator;
            if(dat->isvalue)
            {   if(separator == end)
                {   if(!dat->ignore)
                        tocopy = 1;   } else if(end - separator <= 3)
                {   dat->last_linefeed = end - separator;
                    if(end - separator == 3)
                    {   dat->last_linefeed *= separator[2] == '\r';
                        dat->isvalue = isspace(separator[2]) != 0;   }
                    if(!dat->ignore)
                        tocopy = 1;   } else if(separator[2] == '\r' && separator[3] == '\n')
                {   dat->body = 1;
                    cpbegin = old;
                    cpend = separator;
                    separator = end;   } else if(!isspace(separator[2]))
                {   if(!dat->ignore)
                        tocopy = 3;
                	cpbegin = old;
                	cpend = separator;
                    dat->isvalue = 0;
                    dat->ignore = 0;   }   } else
            {   dat->isvalue = separator != end;
                cpbegin = old;
                cpend = separator;
                tocopy = dat->isvalue + 1;   }
            if(tocopy)
            {   cpcss_partial_copy(dat, tocopy, cpbegin, cpend, out);
                tocopy = 0;   }   }   }
    if(dat->body)
    {   cpcss_partial_copy(dat, 3, cpbegin, cpend, out);
        remaining = len - (cpend + 4 - ptr);   }
    return remaining;   }

int cpcss_parse_http_stream(cpcio_istream is, pcpcss_http_req out)
{   return cpcss_parse_http_stream_ex(is, out, LONG_MAX, LONG_MAX, NULL);   }

int cpcss_parse_http_stream_ex(cpcio_istream is, pcpcss_http_req out, long timelimit, size_t charlimit, cpcpcss_http_req filter)
{   struct cpcss_partial_parse_data parser;
    int succ = cpcss_init_partial_parser(&parser, 32768);
    if(succ == 0)
    {   size_t count, start;
        size_t back;
        size_t total = 0;
        struct timeval tv = {timelimit / 1000000, timelimit % 1000000};
        int ready = 1;
        while(!cpcio_eof_is(is) && total < charlimit && (tv.tv_sec != 0 || tv.tv_usec != 0) && !parser.body)
        {   count = 0;
            start = is->bufi == is->bufs ? 0 : is->bufi;
            if(ready > 0)
            {   while(!cpcio_eof_is(is) && (is->bufi < is->bufs || count == 0))
                {   cpcio_getc_is(is);
                    ++count;   }
                back = cpcss_partial_parse_header(&parser, is->cbuf + start, count, out);
                is->bufi -= back;
                total += count - back;   } else if(ready < 0)
            {   tv.tv_sec = 0;
                tv.tv_usec = 0;
                succ = 1;   }  }
        if(!parser.body)
            succ = 1;
        cpcss_free_partial_parser(&parser);   }
    return succ;   }

int cpcss_parse_http_string(const char *str, pcpcss_http_req out)
{
    struct cpcss_partial_parse_data parser;
    int succ = cpcss_init_partial_parser(&parser, 32768);
    if(succ == 0)
    {   const char *strend = str + strlen(str);
        const unsigned bytes = 16384;
        for(; str + bytes < strend; str += bytes)
            cpcss_partial_parse_header(&parser, str, bytes, out);
        cpcss_partial_parse_header(&parser, str, strend - str, out);
        cpcss_free_partial_parser(&parser);   } else
    succ = CPCSS_REQ_MEMORY_ERROR;
    return succ;
}

void cpcss_free_request(pcpcss_http_req this)
{   cpcss_free_response(this);
    if(this->rru.req.requrl != NULL)
        free(this->rru.req.requrl);   }

void cpcss_free_response(pcpcss_http_req this)
{   if(this->headers != NULL)
	{   for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
        {   if(*it != NULL)
                free(*it);   }
        free(this->headers);   }
    if(this->body != NULL)
        free(this->body);   }

#endif
#endif
