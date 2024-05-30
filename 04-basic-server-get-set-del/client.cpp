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
#include <string>
#include <vector>

// Prints messages to standard error
static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}
// Prints an error message and aborts the program
static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

// Reads exactly 'n' bytes from a file descriptor 'fd' into 'buf'
static int32_t read_full(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or unexpected EOF
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}
// Writes exactly 'n' bytes from 'buf' to a file descriptor 'fd'
static int32_t write_all(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

const size_t k_max_msg = 4096;  // Maximum message size

// Sends a request to the server
static int32_t send_req(int fd, const std::vector<std::string> &cmd) {
    uint32_t len = 4; // Start with the length of the command count
    for (const std::string &s : cmd) {
        len += 4 + s.size();  // Add the length of each string plus its size prefix
    }
    if (len > k_max_msg) {
        return -1;  // Message too long
    }

    char wbuf[4 + k_max_msg];  // Write buffer
    memcpy(&wbuf[0], &len, 4);  // assume little endian; Copy total length to the first 4 bytes
    uint32_t n = cmd.size();  // Number of commands
    memcpy(&wbuf[4], &n, 4);  // Copy the command count next
    size_t cur = 8; // Current position in buffer
    for (const std::string &s : cmd) {
        uint32_t p = (uint32_t)s.size();  // Size of the string
        memcpy(&wbuf[cur], &p, 4);  // Copy string size
        memcpy(&wbuf[cur + 4], s.data(), s.size());  // Copy string data
        cur += 4 + s.size();  // Move position
    }
    return write_all(fd, wbuf, 4 + len);  // Write the entire buffer to the socket
}

// Reads a response from the server
static int32_t read_res(int fd) {
    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];  // Read buffer
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);  // Read the length of the response
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4);  // assume little endian; Get the length from the buffer
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // reply body
    err = read_full(fd, &rbuf[4], len);  // Read the rest of the response
    if (err) {
        msg("read() error");
        return err;
    }

    // print the result
    uint32_t rescode = 0;
    if (len < 4) {
        msg("bad response");
        return -1;
    }
    memcpy(&rescode, &rbuf[4], 4);  // Read the response code
    printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[8]);
    return 0;
}

int main(int argc, char **argv) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("connect");
    }

    std::vector<std::string> cmd;  // Prepare command vector
    for (int i = 1; i < argc; ++i) {
        cmd.push_back(argv[i]);  // Add command line arguments to command vector
    }
    int32_t err = send_req(fd, cmd);  // Send request to server
    if (err) {
        goto L_DONE;  // If error, skip to clean up
    }
    err = read_res(fd);  // Read response from server
    if (err) {
        goto L_DONE;  // If error, skip to clean up
    }

L_DONE:
    close(fd);  // Close the socket
    return 0;
}