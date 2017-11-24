#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *errorMessage)
{
    perror(errorMessage);
    exit(0);
}

int main(int argc, char *argv[])
{
    int socketFDecrip, portNum, n;
    struct sockaddr_in serv_addr;
    struct hostent *server; //struct that defines a host computer on the internet

    char buffer[256];
    if(argc < 3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portNum = atoi(argv[2]);
    socketFDecrip = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFDecrip < 0){
        fprintf(stderr, "ERROR, the host does not exist\n" );
        exit(0);
    }
    server = gethostbyname(argv[1]); //system call that returns a pointer to a hostent struct containing information about that host
    if(server == NULL){
        fprintf(stderr, "ERROR, no such server host\n");
        exit(0);
    }
    bzero((char* )&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    //bcopy copies n bytes from src to destination
    serv_addr.sin_port = htons(portNum);

    //connect() system call is called by the client to establish the connection to the listening server!
    //returns -1 if it fails
    //the port number for the client will be assigned dynamically by the OS on connect() - but obviously need to know port severs listening on...
    if(connect(socketFDecrip, &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR connecting to server");
    }

    printf("Enter a message to send to the server: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin); //reads a line from stdin into buffer
    //use write() system call to write to socket!
    n = write(socketFDecrip, buffer, strlen(buffer));
    if(n < 0){
        error("Error writting to socket :(");
    }
    bzero(buffer, 256);
    //now we block and wait for the servers response, and when it does respond we copy that shit into the buffer
    n = read(socketFDecrip, buffer, 255);
    if(n < 0){
        error("Damnnn son we couldn't read back from the server, socket read failed :(");
    }
    printf("%s\n",buffer);

    return 0;
}