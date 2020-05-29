# CubedProgrammerCSockStreams
C Socket Streams Library
## Required Libraries
CubedProgrammerCInputOutput library at https://github.com/CubedProgrammer/CubedProgrammerCInputOutput

Make sure to add lib and header_only_include of CPCIO in the command as include paths.
#### Opening a server
```
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

#### cpcss_open_server(port)
Param port is a const string that is the port.

Opens a server on a specified port, returns an object of type cpcss_server_sock or NULL if server couldn't be opened.
#### open_client_istream(sock), open_client_ostream(sock), open_server_istream(sock), open_server_ostream(sock)
Param sock is either an object of cpcss_server_sock for opening client streams, or of cpcss_client_sock for opening server streams.

Opens a stream to a server or client.
