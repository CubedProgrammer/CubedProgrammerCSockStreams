#ifndef __cplusplus
#ifndef Included_header_only_cpcss_http_h
#define Included_header_only_cpcss_http_h
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cpcss_http.h>
#define P_CONST_A 375651977
#define P_CONST_B 149753843
#define P_CONST_C 987662329

int cpcss_init_http_request(pcpcss_http_req this, const char *url, uint16_t port)
{   size_t len = strlen(url);
    this->port = port;
    this->body = NULL;
    this->hcnt = 0;
    this->hbuckets = 60;
    this->headers = malloc(sizeof(char *) * this->hbuckets);
    int succ = 0;
    if(this->headers != NULL)
    {   for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
        	*it = NULL;
        this->requrl = malloc(1 + len);
        if(this->requrl != NULL)
        {   strcpy(this->requrl, url);
            char *slash = strchr(this->requrl, '/');
            if(slash != NULL)
            {   *slash = '\0';
                if(cpcss_set_header(this, "host", this->requrl))
                    succ = -1; else
                *slash = '/';   } else
            return cpcss_set_header(this, "host", this->requrl);   } else
        return-1;   } else
    succ = -1;
    return succ;   }

uint64_t cpcss_http____hash_key(const char *key)
{   uint64_t hsh;
    uint64_t x = 0, y = 0, z = 0;
    for(const char *it = key; *it != '\0'; ++it)
    {   x *= P_CONST_A;
        x += P_CONST_B + *it;
        y *= P_CONST_B;
        y += P_CONST_C + *it;
        z *= P_CONST_C;
        z += P_CONST_A + *it;   }
    hsh = x + y + z;
    return hsh;   }

char **cpcss_http____get_bucket(pcpcss_http_req this, const char *key)
{   uint64_t hsh = cpcss_http____hash_key(key);
    hsh %= this->hbuckets;
    char **it;
    char *tmp = NULL;
    for(it = this->headers + hsh; *it != NULL; ++it)
    {   if(strcmp(*it, key) == 0)
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

size_t cpcss_request_size(cpcpcss_http_req this)
{
    char reqmeth[12];
    cpcss_http____req_meth_str(reqmeth, this->meth);
    size_t cnt = strlen(reqmeth) + 1;
    char *pth = strchr(this->requrl, '/');
    if(pth != NULL)
        cnt += strlen(pth) + 11;
    else
        cnt += 12;
    size_t klen;
    for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
    {   if(*it != NULL)
        {   klen = strlen(*it);
            cnt += klen + 2;
            cnt += strlen(*it + klen + 1) + 2;   }   }
    cnt += 2;
    if(this->body != NULL)
    	cnt += strlen(this->body);
    return cnt;
}

void cpcss_request_str(char *str, cpcpcss_http_req this)
{   char *strptr = str;
    cpcss_http____req_meth_str(strptr, this->meth);
    strptr += strlen(strptr);
    *strptr = ' ';
    ++strptr;
    char *pth = strchr(this->requrl, '/');
    if(pth != NULL)
    {   strcpy(strptr, pth);
        strptr += strlen(strptr);   } else
    *strptr++ = '/';
    *strptr++ = ' ';
    strcpy(strptr, "HTTP/1.1\r\n");
    strptr += strlen(strptr);
    size_t klen;
    for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
    {   if(*it != NULL)
        {   klen = strlen(*it);
            strcpy(strptr, *it);
            strptr += klen;
            strcpy(strptr, ": ");
            strptr += 2;
            strcpy(strptr, *it + klen + 1);
            strptr += strlen(*it + klen + 1);
            *strptr++ = '\r';
            *strptr++ = '\n';   }   }
    *strptr++ = '\r';
    *strptr++ = '\n';
    *strptr = '\0';
    if(this->body != NULL)
        strcpy(strptr, this->body);   }

void cpcss_free_request(pcpcss_http_req this)
{   if(this->requrl != NULL)
        free(this->requrl);
	if(this->headers != NULL)
	{   for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
        {   if(*it != NULL)
                free(*it);   }
        free(this->headers);   }
    if(this->body != NULL)
        free(this->body);   }

#endif
#endif
