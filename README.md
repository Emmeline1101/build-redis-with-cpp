# build-redis-with-cpp  
Redis is a memory database, which is a highly performant database that can proceed with large data sets.  
This project follows the guidelines from the website: build-your-own.org.  
The project uses CPP and C programming languages to build Redis. 
# How to Run it
### STEP1
Save the code from the client side and server side, and name it as "client.c" and "server.c".
### STEP2
Run the following commands to compile the file
```shell
g++ server.cpp -o server

g++ client.cpp -o client
```
### STEP3
Run the following commands to run the server and the client
```shell
./server

./client

```

## 01. Connect Server and Client by Using TCP
- **Server:** The server listens for client connections on a specific port, accepts these connections, reads data sent by the client, processes it, and sends a response back to the client.

- **Client:** The client initiates a connection to the server, sends data through this connection, then waits and receives the processed response from the server to complete the interaction.

## 02. Protocol Parsing
To send more msg, it's better for the server to take a bunch of requests from the client at the same time. Therefore, we'd better to implement the protocol that can split requests from apart from the TCP byte stream to obtain the distinct content. 

And a good way to split is by declaring **the length of the request** at the beginning of the request.

## 03. Non-blocking mode
Non-blocking mode is  used here, which is useful when the app needs to react to multiple events without sequential blocking calls. Instead of blocking/waiting, the (read/ write/ connect) operations will do other things without returning an error.

The method is implemented by using the 'fd_set_nb()' function, which sets a socket to non-blocking mode

## 04. Basic Server: get, set, del  
This iteration of the code adds a simple protocol handling mechanism, extending the server's functionality to process specific commands and manage a key-value store. 
### How to run
```shell
# Compile the server
gcc server.c -o server

# Compile the client
gcc client.c -o client

# Run the server
./server

# Run the client with different commands

# Retrieve value of key 'k'
./client get k
# Expected output if 'k' does not exist: "server says: [2]"

# Set the value of key 'k' to 'v'
./client set k v
# Expected output: "server says: [0]"

# Retrieve value of key 'k' again
./client get k
# Expected output: "server says: [0] v"

# Delete the key 'k'
./client del k
# Expected output: "server says: [0]"

# Try to get the deleted key 'k'
./client get k
# Expected output: "server says: [2]"

# Send an unrecognized command
./client aaa bbb
# Expected output: "server says: [1] Unknown cmd"


```