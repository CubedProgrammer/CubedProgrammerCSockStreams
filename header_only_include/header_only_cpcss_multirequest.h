#ifndef __cplusplus
#ifndef Included_header_only_cpcss_multirequest_h
#define Included_header_only_cpcss_multirequest_h
#include<stdarg.h>
#include<sys/select.h>
#include<threads.h>
#include<cpcss_multirequest.h>

struct cpcss_cs_url_pair
{   cpcss_client_sock cs;
    const char *url;   };

struct cpcss_parallel_mr_t
{   struct cpcss_cs_url_pair csurl;
    pcpcss_http_req req;
    cpcss_mr_callback_t cb;   };

int cpcss_parallel_thrd_func(void *arg)
{   struct cpcss_parallel_mr_t *truearg = arg;
    struct cpcss____http_request res;
    int succ = cpcss_make_request(truearg->req, &truearg->csurl.cs, &res);
    if(succ == 0)
    {   truearg->cb(truearg->csurl.url, truearg->csurl.cs, &res);
        cpcss_free_response(&res);
        free(truearg->csurl.cs);   }
    cpcss_free_request(truearg->req);
    free(truearg->req);
	return 0;   }

int cpcss_multirequest_timeout(cpcss_mr_callback_t cb, struct cpcss____http_request *req, int ms, ...)
{   typedef const char *str_t;
	struct cpcss____http_request res;
	size_t len, cscnt, cscapa;
	fd_set fds, *fdsp = &fds;
	struct timeval tv, *tvp = &tv;
	cpcss____sh sock;
    int maxi, succ = 0;
    va_list urlls;
    cpcss_client_sock curr;
    str_t url;
    cscnt = 0;
    cscapa = 10;
    struct cpcss_cs_url_pair *tmp, *csls = malloc(cscapa * sizeof(*csls));
    va_start(urlls, ms);
    for(url = va_arg(urlls, str_t); url != NULL; url = va_arg(urlls, str_t))
    {   len = strlen(url);
        if(req->rru.req.requrl != NULL)
            free(req->rru.req.requrl);
        req->rru.req.requrl = malloc(1 + len);
        if(req->rru.req.requrl != NULL)
        {   strcpy(req->rru.req.requrl, url);
            char *slash = strchr(req->rru.req.requrl, '/');
            if(slash != NULL)
            {   *slash = '\0';
                if(cpcss_set_header(req, "host", req->rru.req.requrl))
                    succ = -1; else
                *slash = '/';   } else
            succ = cpcss_set_header(req, "host", req->rru.req.requrl);   } else
        continue;
        if(cscnt == cscapa)
        {   cscapa += cscapa >> 1;
            tmp = malloc(cscapa * sizeof(*tmp));
            memcpy(tmp, csls, cscnt * sizeof(struct cpcss_cs_url_pair));
            free(csls);
            csls = tmp;   }
        csls[cscnt].url = url;
        succ += cpcss_send_request(req, &csls[cscnt].cs) == 0;
        ++cscnt;   }
    va_end(urlls);
    tv.tv_sec = ms / 1000;
    tv.tv_usec = ms % 1000 * 1000;
    maxi = 0;
    FD_ZERO(fdsp);
    for(size_t i = 0; i < cscnt; ++i)
    {   if(csls[i].cs == NULL)
            continue;
        sock = *cpcss_client_socket_get_server(csls[i].cs);
        maxi = sock > maxi ? sock : maxi;
	    FD_SET(sock, fdsp);   }
    for(int i = succ; i > 0;)
    {   if(select(maxi + 1, fdsp, NULL, NULL, tvp) == 0)
            i = 0;
        else
        {   for(size_t j = 0; j < cscnt; ++j)
            {   if(csls[j].cs == NULL)
                    continue;
                sock = *cpcss_client_socket_get_server(csls[j].cs);
                if(FD_ISSET(sock, fdsp))
                {   FD_CLR(sock, fdsp);
                    curr = csls[j].cs;
                    url = csls[j].url;
                    csls[j].cs = NULL;
                    --i;
                    if(cpcss_read_response(&curr, &res) != 0)
                    {   --succ;
                        cb(url, NULL, NULL);   } else
                    cb(url, curr, &res);
                    cpcss_free_response(&res);
                    free(curr);   } else
                FD_SET(sock, fdsp);   }
            tv.tv_sec = ms / 1000;
            tv.tv_usec = ms % 1000 * 1000;   }   }
    free(csls);
    return succ;   }

int cpcss_multirequest_timeout_parallel(cpcss_mr_callback_t cb, struct cpcss____http_request *req, int ms, ...)
{   typedef const char *str_t;
	size_t cscnt, cscapa;
    va_list urlls;
    str_t url;
    thrd_t th;
    int succ = 0;
    cscnt = 0;
    cscapa = 10;
    struct cpcss_parallel_mr_t *tmp, *csls = malloc(cscapa * sizeof(*csls));
    va_start(urlls, ms);
    for(url = va_arg(urlls, str_t); url != NULL; url = va_arg(urlls, str_t))
    {   if(cscnt == cscapa)
        {   cscapa += cscapa >> 1;
            tmp = malloc(cscapa * sizeof(*tmp));
            memcpy(tmp, csls, cscnt * sizeof(struct cpcss_parallel_mr_t));
            free(csls);
            csls = tmp;   }
        csls[cscnt].req = malloc(sizeof(*req));
        if(csls[cscnt].req != NULL)
        {   cpcss_http_cpy(csls[cscnt].req, req);
            csls[cscnt].req->rru.req.requrl = malloc(strlen(url) + 1);
            strcpy(csls[cscnt].req->rru.req.requrl, url);
            char *slash = strchr(csls[cscnt].req->rru.req.requrl, '/');
            if(slash != NULL)
            {   *slash = '\0';
                if(cpcss_set_header(csls[cscnt].req, "host", csls[cscnt].req->rru.req.requrl))
                    succ = -1; else
                *slash = '/';   } else
            succ = cpcss_set_header(csls[cscnt].req, "host", csls[cscnt].req->rru.req.requrl);   } else
        continue;
        csls[cscnt].csurl.url = url;
        csls[cscnt].cb = cb;
        thrd_create(&th, cpcss_parallel_thrd_func, csls + cscnt);
        thrd_detach(th);
        ++cscnt;   }
    va_end(urlls);
    return succ;   }

#endif
#endif
