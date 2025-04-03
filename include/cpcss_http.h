#ifndef __cplusplus
#ifndef Included_cpcss_http_h
#define Included_cpcss_http_h
#include<stdint.h>
#include<cpcio_istream.h>
#include<cpcio_ostream.h>
#include<cpcss_socket.h>

// request methods
#define CPCSS_GET 997
#define CPCSS_PUT 998
#define CPCSS_HEAD 999
#define CPCSS_POST 1000
#define CPCSS_PATCH 1001
#define CPCSS_TRACE 1002
#define CPCSS_DELETE 1003
#define CPCSS_CONNECT 1004
#define CPCSS_OPTIONS 1005

// request errors
#define CPCSS_REQ_MEMORY_ERROR -97
#define CPCSS_REQ_CONNECTION_ERROR -98
#define CPCSS_REQ_MESSAGE_ERROR -99
#define CPCSS_REQ_TIMEOUT_ERROR -100
#define CPCSS_REQ_DOMAIN_ERROR -101

typedef uint16_t cpcss_req_method_t;
typedef uint16_t cpcss_res_code_t;

struct cpcss_partial_parse_data
{
    char*dat;
    unsigned field_size_limit;
    unsigned cnt;
    char isvalue;
    char ignore;
    char last_linefeed;
    char body;
};

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

// initializes just the headers of an HTTP request/response
// returns zero on success
int cpcss_init_http_headers(pcpcss_http_req this);

// deep copies the src object to the dest, anything originally in dest will be overwritten
// and thus, there will be a memory leak if dest was an existing unfreed request or response
// returns zero on success
int cpcss_http_cpy(pcpcss_http_req dest, cpcpcss_http_req src);

// parses a request method string as a number
// the string must be in upper case, such as GET, PUT, etc
// returns zero on success
int cpcss_http_req_meth_num(const char *str, cpcss_req_method_t *meth);

// writes the request method as a string
// request method will be all upper case, such as GET, PUT, etc
// returns zero on success
int cpcss_http_req_meth_str(char *str, cpcss_req_method_t meth);

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

// makes the request to an http server, synchronously, blocks until the server responds
// res will be filled with the server's response
// returns zero on success
// on error, returns either CPCSS_REQ_MESSAGE_ERROR, CPCSS_REQ_CONNECTION_ERROR, or CPCSS_REQ_MEMORY_ERROR
// indicating either the response message was invalid, connection failed, or memory allocation failed
// note invalid response message could also mean there isn't enough memory, but only for the response
int cpcss_make_request_sync(cpcpcss_http_req this, pcpcss_http_req res, cpcio_istream is, cpcio_ostream os);

// Send the request but do not wait for response
// returns zero on success
int cpcss_send_request(cpcpcss_http_req this, cpcio_ostream os);

// Connects to a server so a request can be made
// simple TCP connection to the server, no data sent otherwise
// the socket will be returned to cs
// returns zero on success
int cpcss_connect_http(cpcpcss_http_req this, cpcss_socket *cs);

// parses raw request from a stream
// remember to free the request
// returns zero on success
int cpcss_parse_request(cpcio_istream is, pcpcss_http_req req);

// parses raw request from a stream
// automatically fails if peer takes longer than tlimit milliseconds to respond
// or the header has more than climit characters
// remember to free the request
// returns zero on success
int cpcss_parse_request_ex(cpcio_istream is, pcpcss_http_req req, long tlimit, size_t climit, cpcpcss_http_req filter);

// parses raw response from a stream
// remember to free the response
// returns zero on success
int cpcss_parse_response(cpcio_istream is, pcpcss_http_req res);

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

// initializes the partial parser with a maximum header field length
// returns 0 on success
int cpcss_init_partial_parser(struct cpcss_partial_parse_data *dat, unsigned field_limit);

// parse a part of the request or response
// the partial parse data struct must be initialized
// returns number of remaining bytes in the buffer
unsigned cpcss_partial_parse_header(struct cpcss_partial_parse_data *dat, const char *ptr, unsigned len, pcpcss_http_req out);

// parses the headers from a stream
// the body is not parsed, the last four bytes read will be CR LF CR LF
int cpcss_parse_http_stream(cpcio_istream is, pcpcss_http_req out);

// parses the headers from a stream
// automatically failed if the headers has too many characters
// or the peer takes too long to respond, unit is milliseconds
// the body is not parsed, the last four bytes read will be CR LF CR LF
int cpcss_parse_http_stream_ex(cpcio_istream is, pcpcss_http_req out, long timelimit, size_t charlimit, cpcpcss_http_req filter);

// parses the headers from a null-terminated string
// the string MUST only contain the header and not the body
// specifically, the last four characters must be CR LF CR LF
int cpcss_parse_http_string(const char *str, pcpcss_http_req out);

// frees resources used by the request object
// if the this pointer points to heap allocated memory, it needs to be freed separately
void cpcss_free_request(pcpcss_http_req this);

// frees resources used by the response object
// if the this pointer points to heap allocated memory, it needs to be freed separately
void cpcss_free_response(pcpcss_http_req this);

#endif
#endif
