/*
Name: Ebony Cross-Williams
Course: CS250
Instructor: Jason M Pittman
Due Date: 02/20/2017
Institute: Capitol Technology University
Description: create
Filenam:echoClientTCP.c
Directory: ECross_Hw1

Created by ebony cross on 2/19/17.
Copyright © 2017 ebony cross. All rights reserved.

/Users/ecross/Desktop/server/server.c
Client echo code based on visual lectures, citation accredited below:
TheSecurityTube. (May 09,2012). TCP Echo Client Using Sockets. [Video File].
Retrieved from https://youtu.be/FRm9nk9ooC8?list=PLLfyYMQQoe3vIm2LVHVxh5AYU_Z2cYGLw
*/


#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include <netdb.h>

#define ERROR -1
#define BUFFER 1024

//prototypes
int hostname_to_ip(char *, char*);

struct hostent *he;

int main(int argc, char ** argv){
    //if (argc < 3) {
      //  perror("Usage: %s [website] [port]\n", argv[0]);
        //exit(1);
    //}

    struct sockaddr_in echoServer; //contains ip and port # of the tcp server
    int sock; //connects to the server
    char finput[BUFFER]; //user input in buffer
    char fouput[BUFFER]; //output from server
    char buffer[BUFFER];
    int len, sent;
    char * hostname = argv[2];
    int port = atoi(argv[1]);
    char ip_address[100];
    char *message;
    
    
    hostname_to_ip(hostname, ip_address);
    /*
    
    if((he = gethostbyname(hostname)) == NULL){
        herror("gethostbyname");
        exit(1);
    }
    
    */
    
    
    

    bzero((char *) &echoServer, sizeof(echoServer));
    bcopy(he->h_addr, &echoServer.sin_addr, he->h_length);
    echoServer.sin_port = htons(port); //port # as 1st argument converted to network byte
    echoServer.sin_family = AF_INET; //set address family
    
    
    printf("hostname %s resolve to ip address %s and port #%d\n", hostname,ip_address, port);

    /*create socket for connection*/
    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("Socket error");
        exit(-1);
    }
    
    printf("The socket has been created\n");
    
    
    

    int opt_val = 1;
    setsockopt(sock, IPPROTO_TCP, SO_REUSEADDR, &opt_val, sizeof opt_val); //set options for socket. *Referenced by Professor Pittman

    
    if(sock == -1){
        perror("setsockopt");
        exit(1);
    }
    
    
    printf("...attempted to connect to socket\n");
    /*create TCP connection to echo server */
    if((connect(sock, (struct sockaddr *)&echoServer, sizeof(struct sockaddr_in))) == -1)
    {
        perror("connection error"); //failed connection
        exit(-1);
    }
    else{
        printf("\nSuccessful connection\n");
    }
    
    
    
    printf("The client is now connected to the server\n");

    
    
    //Send some data
    //sprintf(message,"GET HTTP/1.1\r\nHost: %s\r\n\r\n", hostname);
    
    message = "GET HTTP/1.0\r\n";

    printf("the message to send is %s\n", message);
    //fgets(finput, BUFFER, stdin); //gives message to server from user input
    //sent = write(sock, message, strlen(message)); //send user input to server
    
    send(sock, message, strlen(message));
    bzero(buffer, BUFFER);
    
    
    printf("message sent\n");
    
    
    while(read(sock, buffer, BUFFER- 1) != 0)
    {
        fprintf(stderr, "%s", buffer);
        bzero(buffer, BUFFER);
    }
        
        /*read message from server and print out on client side*/
    
        /*
    
        len = recv(sock, fouput, BUFFER, 0); //read server's reply
        fouput[len] = '\0'; //last byle + null character sent by echo server
         
        printf("The response from the server is: %s\n", fouput);
        fputs(fouput, stdout); //print response from echo server
         */
    

    printf("Terminating connection to server");
    
    close(sock); //close connection
    
} //end of main program


/** Hostname-to-ip address Function
 -retrieve info about domain numae and convert to its ip address
 **/
int hostname_to_ip(char * hostname , char* ip)
{
 
    struct in_addr **addr_list; //pointer to an array of pointers
    int i;
    
    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname error");
        return 1;
    }
    
    printf("official hostname: %s\n", he ->h_name);
    /*
    //print out list of aliases
    for( addr_list = he -> h_aliases; *addr_list != NULL; addr_list++){
        printf("\t alias: %s\n", *addr_list);
    }
    */

    addr_list = (struct in_addr **) he -> h_addr_list;
    
    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
    
    return 1;
}

