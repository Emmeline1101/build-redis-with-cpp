//import C library
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

// if there is an error, it will be caught
static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main() {
    // obtain a socket fd
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }
    //connect client to the port and server address (so, the client know the place where the request can be received)
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234); //port
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1 //know up address
    //use connect keywords to connect with server
    //addr to const struct sockaddr *(pointer)
    // sockaddr_in stand for IPv4
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr)); //send the req to the server addr and port
    if (rv) {
        die("connect");
    }

    char msg[] = "hello";
    //use write to send a message to server
    write(fd, msg, strlen(msg));

    char rbuf[64] = {};
    // use read to read the message from server
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        die("read");
    }
    // close socket and quit
    printf("server says: %s\n", rbuf);
    close(fd);
    return 0;
}
