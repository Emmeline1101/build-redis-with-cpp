# build-redis-with-cpp  
Redis is a memory database, which is a highly performant database that can proceed with large data sets.  
This project follows the guidelines from the website: build-your-own.org.  
The project uses CPP and C programming languages to build Redis. 

# Demo
To see more details, please refer to the `img` (`build-redis-with-cpp/img/`) folder. 

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

# Try to delete 'k' again, which doesn't exist anymore, 0 means no key is deleted
./client del k
# Output: (int) 0

# List keys after deletion
./client keys
# Output: (arr) len=0
#         (arr) end

```

## 07. AVL
AVL trees is used to implement the sorted sets in Redis, by utilizing its balancing mechanism. The focus is on ensuring efficient data access and manipulation within Redis, with AVL trees providing a self-balancing binary search tree structure that maintains sorted data in a way that all insert, delete, and search operations are in logarithmic time.

### how to run
```shell
g++ -std=c++11 -o test-avl avl.cpp test-avl.cpp

./test-avl

```

## 08. Sorted Set From AVL Tree  
A sorted set data structure using AVL trees is implemented. By introducing a complex data type, operations can be handled efficiently based on both score and value. 

### Improvements

1. **Enhanced Data Structures**: Using AVL trees provides a robust foundation for implementing sorted sets, which are more complex than the simple key-value or hashmap data structures explored in earlier chapters.  
2. **Efficient Querying**: The introduction of the ZQUERY command and AVL tree's balancing features allows for efficient range queries and rank calculations, which are critical for high-performance database operations.  
3. **Intrusive Data Structures**: Reduces memory overhead and increases performance by avoiding separate allocations for nodes that are part of multiple data structures.  

### File Structure
1. **AVL Tree (avl.cpp, avl.h)** - Implements the AVL tree, a self-balancing binary search tree, ensuring that operations like insertions, deletions, and lookups remain efficient (O(log n) time complexity). This structure supports the sorted set operations by maintaining elements in a sorted order based on score.  

2. **Sorted Set (zset.cpp, zset.h)** - Introduces a sorted set implementation using AVL trees. This involves managing a dual index system:  

- Hashmap index for fast access by name.  
- AVL tree index for ordered traversal by score.  
3. **Client and Server Files (client.cpp, server.cpp)** - Handle the network interaction and command processing, enabling testing and interaction with the sorted set operations through a simplified command interface like ZADD, ZREM, ZQUERY, etc.  

4. **Test Files (test_cmds.py, test_offset.cpp)** - Provide automated testing for command processing and AVL tree operations. This ensures the sorted set operations work correctly and efficiently across various scenarios.

5. **Common Utilities (common.h)** - Contains utility functions and definitions used across different parts of the project.

### How to Run
```shell
g++ -std=c++11 -o server server.cpp avl.cpp hashtable.cpp zset.cpp
g++ -std=c++11 -o client client.cpp

./server

./client ZADD myset 1 "hello"
./client ZADD myset 2 "world"
./client ZQUERY myset 1 "hello" 0 10
./client ZREM myset "hello"
./client ZSCORE myset "world"

```

## 09. The Event Loop and Timers
For this step, server management is enhanced by introducing timeouts and timers to efficiently handle idle TCP connections, which is critical for maintaining network application stability.  

Modifications to the event loop integrate a timeout mechanism that adjusts based on the closest timer, ensuring timely response to inactive connections. The timers are managed using a simple linked list that optimizes for quick updates and maintenance, streamlining the process by appending renewed timers to the end of the list. This approach minimizes resource wastage and boosts server performance by automatically disconnecting idle connections.

### File Structure
**avl.cpp** - Contains the implementation of AVL tree operations such as insertion, deletion, and balancing. This file includes the core functionality for maintaining the AVL tree structure used in the sorted set.

**avl.h** - Header file for avl.cpp, declaring the AVL tree node structure and associated functions, providing the interface for AVL tree operations.

**client.cpp** - Implements the client-side logic for interacting with the server, sending commands and processing responses related to the sorted set operations.

**common.h** - Likely contains common definitions and utility functions used across multiple files in the project, such as error handling or specific constants.

**hashtable.cpp** - Implements the hashtable used in managing indexes within the sorted set, particularly for quick access by key names.

**hashtable.h** - Header file for hashtable.cpp, declaring structures and functions for the hashtable operations.

**server.cpp** - Contains the main server logic handling incoming client requests, integrating AVL tree and hashtable operations to perform sorted set commands.

**zset.cpp** - Provides the implementation of sorted set specific operations such as adding, removing, and querying elements based on scores and other criteria.

**zset.h** - Header file for zset.cpp, declaring the structures and functions specific to the sorted set operations.

**test_cmds.py** - A Python script used for automated testing of server commands, verifying that sorted set operations perform as expected.

**test_offset.cpp** - Contains tests specifically designed to verify the correctness of offset calculations within the AVL tree, ensuring that the tree maintains correct order statistics.


### How to Run
```shell
# compile the files
g++ -std=c++11 -o server server.cpp avl.cpp hashtable.cpp heap.cpp zset.cpp

# Run the server
./server

g++ -std=c++11 -o test_heap test_heap.cpp heap.cpp
./test_heap

```
## 10. The Heap Data Structure & TTL (time to live)
Heap data structures is introduced to handle timeouts and TTLs (time to live) for cached data, which is crucial for efficiently managing the expiration of data in a cache server like Redis. This implementation allows for more flexible and variable timer durations compared to the fixed-value timers used in the linked lists of the previous setup.### How to Run

```shell
# compile the files
g++ -std=c++11 -o server server.cpp avl.cpp hashtable.cpp heap.cpp zset.cpp

# Run the server
./server
