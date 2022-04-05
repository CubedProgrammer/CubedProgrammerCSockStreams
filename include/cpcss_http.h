#ifndef __cplusplus
#ifndef Included_cpcss_http_h
#define Included_cpcss_http_h
#include<stdint.h>

#define CPCSS_GET 997
#define CPCSS_PUT 998
#define CPCSS_HEAD 999
#define CPCSS_POST 1000
#define CPCSS_PATCH 1001
#define CPCSS_TRACE 1002
#define CPCSS_DELETE 1003
#define CPCSS_OPTIONS 1004
#define CPCSS_CONNECT 1005

typedef uint16_t cpcss_req_method_t;

// struct for http request
struct cpcss____http_request
{   cpcss_req_method_t meth;
    char *requrl;
    uint16_t port;
    char **headers;
    size_t hcnt, hbuckets;
    char *body;   };

// typedefs for the struct
typedef struct cpcss____http_request cpcss_http_req, *pcpcss_http_req;
typedef const struct cpcss____http_request *cpcpcss_http_req;

// initializes an http request struct
// url must not have the leading http://
// returns zero on success
int cpcss_init_http_request(pcpcss_http_req this, const char *url, uint16_t port);

// sets a header of the request
// returns zero on success
int cpcss_set_header(pcpcss_http_req this, const char *key, const char *val);

// adds a header to the request
// if the key already exists, val will be appended to the end of the existing val
// with a comma and a space preceding it
// returns zero on success
int cpcss_add_header(pcpcss_http_req this, const char *key, const char *val);

// erases a header entry by its key
// returns zero on success
int cpcss_erase_header(pcpcss_http_req this, const char *key);

// makes the request to an http server
// returns zero on success
int cpcss_make_request(cpcpcss_http_req this);

// gets the size of the request in bytes if it were to be sent
// allocate this size plus one for str of cpcss_request_str
size_t cpcss_request_size(cpcpcss_http_req this);

// writes request to a string
// str must have the capacity to store the request
void cpcss_request_str(char *str, cpcpcss_http_req this);

// frees resources used by the request object
// if the this pointer points to heap allocated memory, it needs to be freed separately
void cpcss_free_request(pcpcss_http_req this);

#endif
#endif
