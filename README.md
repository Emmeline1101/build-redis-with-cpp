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
# Compile the server & client

g++ -std=c++11 server.cpp -o server
g++ -std=c++11 client.cpp -o client

# Run the server
./server

# Run the client with different commands; [2] - error, [0] - ok

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

## 05. Hashtable

The improvements allow Redis to scale effectively, handling hundreds of GBs with minimal latency impact.  

Redis is significantly enhanced by utilizing two primary classes of hashtables: open addressing and chaining. Open addressing is preferred for its CPU cache efficiency and lower memory overhead, while chaining handles collisions better, especially with intrusive data structures that reduce memory allocations.   

This dual approach allows Redis to efficiently manage latency and memory usage even under heavy loads, ensuring stable performance through thoughtful handling of worst-case scenarios and avoiding the complexity of pointer indirection in hot paths.   

### why use C here?
C offers low-level access to memory and minimal abstraction overhead

### structure of files
1. **hashtable.h**: Defines the structures and functions for the hash table implementation.  
2. **hashtable.cpp**: Contains the implementation of the hash table functions such as initialization, insertion, lookup, and deletion.  
3. **server.cpp**: Uses the hash table to create a server that can handle basic commands like set, get, and del to manipulate key-value pairs stored in the hash table.  

### how to Run?
```shell
# Compile the code
g++ -std=c++11 -o server server.cpp hashtable.cpp
# Run the server
./server
```

## 06. Data Serialization
A robust serialization protocol is made to enhance the server's protocol to handle more complex data types and efficiently manage command responses. Below is a more detailed explanation:  
1. **Enhanced Data Types**: The introduction of a serialization protocol supports five primary data types: nil, error messages, strings, integers, and arrays, which  allows Redis to handle more complex data structures and responses
2. **Serialization Protocol**: The serialization approach adopted is the Type-Length-Value (TLV) format, which facilitates the encoding of not only flat data but also nested structures.
data structures and responses
3. **Command Response Flexibility**: By leveraging the serialization protocol, Redis can now return detailed and structured responses based on the command executed.  
### how to  run
```shell
# Compile the server
g++ -std=c++11 -o server server.cpp hashtable.cpp

# Compile the client
g++ -std=c++11 -o client client.cpp

# start server
./server

# Testing an unknown command
./client asdf
# Output: (err) 1 Unknown cmd

# Attempt to get a non-existing key
./client get asdf
# Output: (nil)

# Set a key 'k' with the value 'v'
./client set k v
# Output: (nil) - assuming your setting doesn't give a confirmation message

# Get the value of key 'k'
./client get k
# Output: (str) v

# List all keys
./client keys
# Output: (arr) len=1
#         (str) k
#         (arr) end

# Delete the key 'k'
./client del k
# Output: (int) 1

# Try to delete 'k' again, which doesn't exist anymore
./client del k
# Output: (int) 0

# List keys after deletion
./client keys
# Output: (arr) len=0
#         (arr) end

```