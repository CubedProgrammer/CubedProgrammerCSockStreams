# CubedProgrammerCSockStreams
C Socket Streams Library
## Required Libraries
CubedProgrammerCInputOutput library at https://github.com/CubedProgrammer/CubedProgrammerCInputOutput

Make sure to add lib and header_only_include of CPCIO in the command as include paths.
## Sample compilation using clang
Assuming CPCIO repository is in /usr/include/cpcio, which also contains libcpcio.so
```sh
clang -I/usr/include/cpcio/include -I/usr/include/cpcio/header_only_include -Iinclude -Iheader_only_include -O3 -c cpcss_socket.c cpcss_sockstream.c cpcss_http.c cpcss_multirequest.c -fPIC
clang -shared -o libcpcss.so cpcss_socket.o cpcss_sockstream.o cpcss_http.o cpcss_multirequest.o -L /usr/include/cpcio -lcpcio
```
On POSIX compliant operating systems, there are directories for includes and shared libraries usually /usr/local/include or /usr/local/lib.
Therefore, putting the headers and shared libraries in those directories is highly desirable.

On Windows, there are no such directories, consider creating them yourselves and use shell scripts to help with compilation.
Windows support is also limited and comes as a second thought, but pull requests are always welcome.
#### Opening a server
```c
#include <cpcss_socket.h>
#include <sockstream.h>
int main(void)
{
    cpcss_server_sock sock = cpcss_open_server("7777");// opens a server on port 7777 and accepts a client
    istream *is = open_client_istream(sock);// opens an istream to the client
    ostream *os = open_client_ostream(sock);// opens an ostream to the client
    return 0;
}
```

#### Type cpcss_http_req
Describes an http request or response.

char *body is a string, representing the request or response body.

char *requrl is the url to connect to.

uint16_t rru.req.port is the port number.

cpcss_req_meth_t rru.req.meth is an integer representing the request type, see cpcss_set_req_method.

cpcss_res_code_t rru.res is an integer representing the response code.

#### cpcss_open_server(port)
Param port is a `uint16_t` that is the port.

Opens a server on a specified port, returns an object of type cpcss_server_sock or NULL if server couldn't be opened.

Note that this function opens a server and accepts at least one client before returning
#### cpcss_accept_client(sv)
Param sv is a cpcss_server_sock

Accepts a client, returns the cpcss_client_sock.
#### cpcss_connect_client(host,port)
Param host is the host, obviously.

Param port is a `uint16_t`, which is the port.

Connects a client to a server.
#### cpcss_close_server(sv)
Param sv is a cpcss_server_sock.

Closes a server socket.
#### cpcss_discon_client(cs)
Param cs is a cpcss_client_sock.

Disconnects a client.
#### cpcss_client_socket_get_server(c), cpcss_server_socket_get_client(s)
Parm c/s is a cpcss_client/server_sock.

Obtains the raw socket handle, directly operating on this is inadvisable.
#### open_client_istream(sock), open_client_ostream(sock), open_server_istream(sock), open_server_ostream(sock)
Param sock is either an object of cpcss_server_sock for opening client streams, or of cpcss_client_sock for opening server streams.

Opens a stream to a server or client.
#### cpcss_init_http_request(this, url, port)
Param this is a cpcpcss_http_req, an HTTP request.

Param url is a string, the url to connect to.

Param port is the port to connect to, a number.

Returns zero on success.
#### cpcss_init_http_response(this, res, body)
Param this is a cpcpcss_http_req, an HTTP request.

Param res is the response code, 100 through 599.

Param body is the response body, a string, may be NULL.

Returns zero on success.
#### cpcss_set_req_method(this, meth)
Param this is a cpcpcss_http_req, an HTTP request.

Param meth is a request method, one of the nine macros defined.
```
CPCSS_GET
CPCSS_PUT
CPCSS_HEAD
CPCSS_POST
CPCSS_PATCH
CPCSS_TRACE
CPCSS_DELETE
CPCSS_CONNECT
CPCSS_OPTIONS
```
Returns zero on success.
#### cpcss_set_header(this, key, val)
Param this is a cpcpcss_http_req, an HTTP request.

Params key and val are the name and value of the field, respectively.

Replaces the old value if it already exists.
Returns zero on success.
#### cpcss_add_header(this, key, val)
Param this is a cpcpcss_http_req, an HTTP request.

Params key and val are the name and value of the field, respectively.

If the field already exists, the value will be appended with a comma and a space.
Returns zero on success.
#### cpcss_erase_header(this, key)
Param this is a cpcpcss_http_req, an HTTP request.

Param key is the header field you wish to erase.

Removes the field in question, completely and utterly.
Returns zero is the field exists and was successfully removed.
#### cpcss_get_header(this, key)
Param this is a cpcpcss_http_req, an HTTP request.

Param key is the name of the field you wish to fetch.

Returns a const char pointer to a string containing the value of the header.
#### cpcss_make_request(this, cs, res)
Param this is a cpcpcss_http_req, the HTTP request to make.

Param cs is a cpcss_client_sock, the socket to the server.

Param res is a pcpcss_http_req, the response received from a server.

Returns zero on success
On error, returns either CPCSS_REQ_MESSAGE_ERROR, CPCSS_REQ_CONNECTION_ERROR, or CPCSS_REQ_MEMORY_ERROR
Indicating either the response message was invalid, connection failed, or memory allocation failed
Note invalid response message could also mean there isn't enough memory, but only for the response
#### cpcss_send_request(this, cs)
Param this is a cpcpcss_http_req, the HTTP request to make.

Param cs is a cpcss_client_sock, the socket to the server.

Returns zero on success.
#### cpcss_read_response(cs, res)
Param cs is a cpcss_client_sock, the socket to the server.

Param res is a pcpcss_http_req, the response received from a server.

Returns zero on success.
#### cpcss_request_size(this)
Param this is a cpcpcss_http_req, an HTTP request.

Returns the length of the string if this is to be converted to a string, excluding null terminator.
#### cpcss_request_str(str, this)
Param str is a buffer, it will store the request as a string.

Param this is a cpcpcss_http_req, the request to convert to string.

Converts request to string, it will be null terminated.
#### cpcss_response_size(this)
Param this is a cpcpcss_http_req, an HTTP response.

Returns the length of the string if this is to be converted to a string, excluding null terminator.
#### cpcss_response_str(str, this)
Param str is a buffer, it will store the response as a string.

Param this is a cpcpcss_http_req, the response to convert to string.

Converts response to string, it will be null terminated.
#### cpcss_parse_response(str, res)
Param str is a string, it should represent a raw HTTP response.

Param res is a pcpcss_http_req, it will store the response headers, body, and code after parsing.

Parses a raw string HTTP response, returns zero if successful, non-zero if the response is invalid.
#### cpcss_free_request(req)
Param req is a pcpcss_http_req.

Frees the memory used by req, if req is heap allocated, it'll have to be freed separately.
#### cpcss_free_response(res)
Param res is a pcpcss_http_req.

Frees the memory used by res, if res is heap allocated, it'll have to be freed separately.
#### Type cpcss_mr_callback_t
Callback type for multirequest.

Pointer to function void(const char *url, cpcss_client_sock, cs, pcpcss_http_req res)
#### cpcss_multirequest(cb, req, ...)
Param cb is the callback function.

Param req is the request to make, URL will be replaced with the URLs given.

All other parameters are urls.

The request can be initialized with any URL, since it will be replaced anyways.
The callback will be called with the URL, the client socket, and the pointer to the response, for all successful connections.
If the response was invalid, the callback will be called with the URL and two NULL pointers. 
Do not free the response or free the pointer to the response.
The calls to the callback will not necessarily be called with the URLs in the order they appear.
However, the requests will be sent to the URLs in the order they appear.
The last argument must be NULL.
