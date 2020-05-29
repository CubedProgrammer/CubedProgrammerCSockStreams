gcc -I./include -I./header_only_include -O3 -c -o cpcss_socket.o cpcss_socket.c
gcc -I./include -I./header_only_include -O3 -c -o sockstream.o sockstream.c
gcc -shared -o cpcss.dll cpcss_socket.o sockstream.o