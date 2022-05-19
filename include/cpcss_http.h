#ifndef __cplusplus
#ifndef Included_cpcss_http_h
#define Included_cpcss_http_h
#include<stdint.h>
#include<cpcss_socket.h>

// request methods
#define CPCSS_GET 997
#define CPCSS_PUT 998
#define CPCSS_HEAD 999
#define CPCSS_POST 1000
#define CPCSS_PATCH 1001
#define CPCSS_TRACE 1002
#define CPCSS_DELETE 1003
#define CPCSS_OPTIONS 1004
#define CPCSS_CONNECT 1005

// request errors
#define CPCSS_REQ_MEMORY_ERROR -97
#define CPCSS_REQ_CONNECTION_ERROR -98
#define CPCSS_REQ_MESSAGE_ERROR -99

typedef uint16_t cpcss_req_method_t;
typedef uint16_t cpcss_res_code_t;

// struct for request information
struct cpcss____http_request_info
{   cpcss_req_method_t meth;
    char *requrl;
    uint16_t port;   };

// request response union
union cpcss____http_rru
{   struct cpcss____http_request_info req;
    cpcss_res_code_t res;   };

// struct for http request
struct cpcss____http_request
{   union cpcss____http_rru rru;
    char **headers;
    size_t hcnt, hbuckets;
    char *body;   };

// typedefs for the struct
typedef struct cpcss____http_request cpcss_http_req, *pcpcss_http_req;
typedef const struct cpcss____http_request *cpcpcss_http_req;

// initializes an http request
// url must not have the leading http://
// request method is GET by default
// returns zero on success
int cpcss_init_http_request(pcpcss_http_req this, const char *url, uint16_t port);

// initializes an http response
// body may be null, otherwise it must be a heap allocated and null-terminated string
// response code of 200 is set in the case that the provided response code is invalid
// returns zero on success
int cpcss_init_http_response(pcpcss_http_req this, cpcss_res_code_t res, char *body);

// sets the request method
// if the request method is not one of the nine macros defined above
// the request pointed to by this remains unchanged, -1 is returned
// return zero on success
int cpcss_set_req_method(pcpcss_http_req this, cpcss_req_method_t meth);

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

// Gets the value of a header field
// Returns NULL if the field does not exist
const char *cpcss_get_header(cpcpcss_http_req this, const char *key);

// makes the request to an http server
// res will be filled with the server's response
// returns zero on success
// on error, returns either CPCSS_REQ_MESSAGE_ERROR, CPCSS_REQ_CONNECTION_ERROR, or CPCSS_REQ_MEMORY_ERROR
// indicating either the response message was invalid, connection failed, or memory allocation failed
// note invalid response message could also mean there isn't enough memory, but only for the response
int cpcss_make_request(cpcpcss_http_req this, cpcss_client_sock *cs, pcpcss_http_req res);

// gets the size of the request in bytes if it were to be sent
// allocate this size plus one for str of cpcss_request_str
size_t cpcss_request_size(cpcpcss_http_req this);

// writes request to a string
// str must have the capacity to store the request
void cpcss_request_str(char *str, cpcpcss_http_req this);

// gets the size of the response in bytes if it were to be sent
// allocate this size plus one for str of cpcss_request_str
size_t cpcss_response_size(cpcpcss_http_req this);

// writes response to a string
// str must have the capacity to store the request
void cpcss_response_str(char *str, cpcpcss_http_req this);

// parses raw response
// returns zero on success
int cpcss_parse_response(const char *str, pcpcss_http_req res);

// frees resources used by the request object
// if the this pointer points to heap allocated memory, it needs to be freed separately
void cpcss_free_request(pcpcss_http_req this);

#endif
#endif
