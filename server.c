#include <stdio.h>
#include <sys/types.h> //includes a number of data types used in system calls
#include <sys/socket.h>//includes for stuff required for sockets!
#include <netinet/in.h> //includes constants and structure for addresses

void error(char *errorMessage){
    perror(errorMessage);
    exit(1);
}

int main(int argc, char const *argv[])
{
    //clientLen = size of the address of the client
    int socketFDescrip, newSocketFDescrip, portNum, clientLen, n;

    char buffer[256]; //server will read characters from the socket connection into this buffer
    struct sockaddr_in serv_addr, cli_addr; //contains addressed for sever, and any client that connects

    if(argc < 2){
        fprintf(stderr, "ERROR, please provide a port\n");
        exit(1);
    }

    //The socket() system call creates a new socket taking 3 args:
        //address domain of the socket: either unix domain, or internet domain.
        //Type of socket, UDP(SOCK_DGRAM) or TCP (SOCK_STREAM)
        //Lastly the protocol - when set to 0 the OS chooses the protocol, in this case it'll choose TCP for us :)
    socketFDescrip = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFDescrip < 0){
        error("Error openning sever socket!");
    }
    //The call to the system call socket(), returns an entry into the file descriptor table (some integer).
    //this is used for all subsequent references to this socket. if it fails it'l return -1, so we've put in a check for this..

    bzero((char *) &serv_addr, sizeof(serv_addr)); // sets all values in a buffer to zero, the first argument is the buffer, and the second the size of the buffer
    //in this case we're setting our server_addr to all zeros

    portNum = atoi(argv[1]); //set our portNumber to the passed in arg, atoi = ASCII to integer

    //initilize server address:
    serv_addr.sin_family = AF_INET; //set code for address family
    serv_addr.sin_port = htons(portNum); //use htons because we need to convert our integer from host byte order to network byte order (relating to endianess)
    serv_addr.sin_addr.s_addr = INADDR_ANY; // set the ip of the host - ip of machine our sever is running on.

    //bind() system call, that binds a socket to an address, in this case: address of the current host & port num that our severse running on
    //Three args:
        //socketFileDescriptor, address to which were binding on, and size of the address
    if(bind(socketFDescrip, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("Server failed to bind socket to address");
    }
    //if it fails as before a negative int will be returned e..g if address already bound to it will fail!

    //Now we want the process to listen on the socket we've created for connections:
    //The scond arg is the backloq queue, i.e number of connections waiting, while process is handeling another
    listen(socketFDescrip, 4);

    clientLen = sizeof(cli_addr);
    //second argument to accept() is a pointer to the client that is connecting to our server. and the third just it's size...
    newSocketFDescrip = accept(socketFDescrip, (struct sockaddr*) &cli_addr, &clientLen);
    //The accept() system call will cause the process: It will block until a client connects to the server.
    //The process will sit idle until a succesfull connection is established by the client
    //again it returns a File descriptor, all communications over this connection should be done by referencing
    //this file descriptor!
    if(newSocketFDescrip < 0){
        error("Error accepting connection from client");
    }

    bzero(buffer, 256); 
    //read() system call will block until theres something to read in the socketFD
    //aka only after the client executes a write() on the socket!
    //read will store the contents written by te client into our buffer[]
    //the last argument is the number of bytes to read from the socket before truncating
    n = read(newSocketFDescrip, buffer, 255);
    //n will now hold the number of bytes read from the socket!
    if(n < 0){
        error("Error reading from connection socket");
    }
    printf("Message Received from client: %s\n", buffer);
    char *genericReply = "I got ya message: ";
    char *reply = malloc(strlen(genericReply)+strlen(buffer)+1);
    strcpy(reply, genericReply);
    strcat(reply, buffer);
    int messageSize = (strlen(reply) + 1 ) * sizeof(char);
    //we can reply to the sever by writting to the socket! 
    //write takes the message, and the messages size
    n = write(newSocketFDescrip,reply ,messageSize);
    if(n < 0){
        error("Error writting to socket :(");
    }

    return 0;
}