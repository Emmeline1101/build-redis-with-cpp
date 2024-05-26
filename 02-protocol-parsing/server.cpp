#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}
const size_t k_max_msg = 4096;

static int32_t read_full(int fd, char *buf, size_t n) { //try to read n bytes from fd to the buffer pointed by buf
    while(n > 0) {
        ssize_t rv  = read(fd, buf, n); // read from fd
        if (rv <= 0) {
            return -1; // error
        }
        assert((size_t) rv <= n); // debugging, if the len of rv > n, the program will terminate
        n -= (size_t) rv; // casting to ensure that rv has the same type with n. And reducing n by the bytes read from rv; n =  n - rv
        buf += rv; // buf is a pointer. here buf points to the buffer where the next set of bytes should be stored; advancing buf by rv

    }
    return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n) { // Writes data from a buffer to a file descriptor 
    while (n > 0)  {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1; //error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t one_request(int connfd) {
    // 4 bytes header
    char rbuf[4 + k_max_msg + 1]; // buffer declaration
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) { // return 0 on success and non-zero on error
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read() error");
        }
        return err;
    }

    uint32_t len = 0; // initializes a variable len of type uint32_t
    memcpy(&len, rbuf, 4);  // assume little endian; copy 4 bytes of data from one location (rbuf) to another (&len)
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    // do something
    rbuf[4 + len] = '\0'; // a header to receive data, 4-byte header, len is the length of the message; here 4 + len calculates the position in the buffer after the  end of the message
    printf("client says: %s\n", &rbuf[4]);

    // reply using the same protocol
    const char reply[] = "world"; //define the Response Message
    char wbuf[4 + sizeof(reply)]; // prepare the Buffer for Sending
    len = (uint32_t)strlen(reply); // calculate the Length of the Message
    memcpy(wbuf, &len, 4); // copy the Length and Message into the Buffer; embeds the length of the message at the beginning of the buffer
    memcpy(&wbuf[4], reply, len); // copies the actual message data from reply into wbuf, starting right after the 4-byte length prefix
    return write_all(connfd, wbuf, 4 + len); //  send the complete message to the client via the connection file
}

int main() {
    // Create a socket of the type internet with TCP protocol
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    // this is needed for most server applications; Set socket options; allows reuse of local addresses
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind; Define the server address structure
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET; // Internet address family
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);    // wildcard address 0.0.0.0;Listen on all interfaces


    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));//bind the socket with the port
    if (rv) {
        die("bind()");
    }

    // Listen on the socket for incoming connections
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

  // Server loop
    while (true) {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);

        // Accept a new connection
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0) {
            continue;   // error; skip the rest of the loop on error
        }

        // Handle requests from the client
        while (true) {
            // here the server only serves one client connection at once
            int32_t err = one_request(connfd); // Process one request
            if (err) {
                break;
            }
        }
         // Close the client socket
        close(connfd);
    }

    return 0;
}
