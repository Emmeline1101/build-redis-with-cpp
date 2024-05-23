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
gcc -o server server.c

gcc -o client client.c
```
### STEP3
Run the following commands to run the server and the client
```shell
./server

./client

```

# 01. Connect Server and Client by Using TCP
- **Server:** The server listens for client connections on a specific port, accepts these connections, reads data sent by the client, processes it, and sends a response back to the client.

- **Client:** The client initiates a connection to the server, sends data through this connection, then waits and receives the processed response from the server to complete the interaction.