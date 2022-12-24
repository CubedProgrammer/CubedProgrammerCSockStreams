#ifndef __cplusplus
#ifndef Included_cpcss_multirequest_h
#define Included_cpcss_multirequest_h
#include<cpcss_http.h>
#define cpcss_multirequest(cb, req, ...)cpcss_multirequest_timeout(cb, req, 15000, __VA_ARGS__)
#define cpcss_multirequest_parallel(cb, req, ...)cpcss_multirequest_timeout_parallel(cb, req, 15000, __VA_ARGS__)
// callback typedef
// Do not free the pointer, that will be handled automatically
// The memory pointed to will be changed after the call returns
// If the client socket is NULL, then the request failed
typedef void(*cpcss_mr_callback_t)(const char*,cpcss_client_sock,struct cpcss____http_request*);

// Make multiple requests, takes arbiturary number of const char pointers, these are URLs
// Wait for specified number of milliseconds
// Final parameter must be a const char pointer to NULL
// The callback will be called for every response, if the request pointer is NULL, connection failed
// The callback order is not necessarily the order the URLs appear
int cpcss_multirequest_timeout(cpcss_mr_callback_t cb, struct cpcss____http_request *req, int ms, ...);

// Same thing but parallel
int cpcss_multirequest_timeout_parallel(cpcss_mr_callback_t cb, struct cpcss____http_request *req, int ms, ...);

#endif
#endif
