#ifndef __cplusplus
#ifndef Included_header_only_cpcss_http_h
#define Included_header_only_cpcss_http_h
#include<stdlib.h>
#include<cpcss_http.h>

int cpcss_init_http_request(pcpcss_http_req this, const char *url, uint16_t port)
{
	size_t len = strlen(url);
	this->port = port;
	this->requrl = malloc(1 + len);
	if(this->requrl != NULL)
	{
		strcpy(this->requrl, url);
		char *slash = strchr(this->requrl, '/');
		if(this->requrl != NULL)
		{
			*slash = '\0';
			cpcss_set_header(this, "host", this->requrl);
			*slash = '/';
		}
		else
			return-1;
	}
	else
		return-1;
	return 0;
}

int cpcss_set_header(pcpcss_http_req this, const char *key, const char *val);

int cpcss_add_header(pcpcss_http_req this, const char *key, const char *val);

int cpcss_erase_header(pcpcss_http_req this, const char *key);

size_t cpcss_request_size(cpcpcss_http_req this);

void cpcss_request_str(char *str, cpcpcss_http_req this)
{
	char *strptr = str;
	size_t klen;
	for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
	{
		if(*it != NULL)
		{
			klen = strlen(*it);
			strcpy(strptr, *it);
			strptr += klen;
			strcpy(strptr, ": ");
			strptr += 2;
			strcpy(strptr, *it + klen + 1);
			strptr += strlen(*it + klen + 1);
			*strptr++ = '\r';
			*strptr++ = '\n';
		}
	}
	*strptr++ = '\r';
	*strptr++ = '\n';
	if(this->body != NULL)
		strcpy(strptr, this->body);
}

void cpcss_free_request(pcpcss_http_req this)
{
	if(this->requrl != NULL)
		free(this->requrl);
	if(this->headers != NULL)
	{
		for(char **it = this->headers; it != this->headers + this->hbuckets; ++it)
		{
			if(*it != NULL)
				free(*it);
		}
		free(this->headers);
	}
	if(this->body != NULL)
		free(this->body);
}

#endif
#endif
