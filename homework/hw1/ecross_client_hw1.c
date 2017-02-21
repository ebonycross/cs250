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

#define ERROR -1
#define BUFFER 1024

int main(int argc, char ** argv){

    struct sockaddr_in echoServer; //contains ip and port # of the tcp server
    int sock; //connects to the server
    char finput[BUFFER]; //user input in buffer
    char fouput[BUFFER]; //output from server
    int len, sent;
    int port = atoi(argv[2]);
    
    /*create socket for connection*/
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("Socket error");
        exit(-1);
    }

    echoServer.sin_family = AF_INET; //set address family
    echoServer.sin_port = htons(port); //port # as 1st argument converted to network byte order
    echoServer.sin_addr.s_addr = inet_addr(argv[1]); //ip address is ascii form
    
    /*create TCP connection to echo server */
    if((connect(sock, (struct sockaddr *)&echoServer, sizeof(struct sockaddr_in))) == ERROR)
    {
        perror("connection error"); //failed connection
        exit(-1);
    }
    
    printf("The client is now connected to the server\n");
    
    //connection was successful
   while(1)
    //while(fgets(finput, BUFFER, stdin) != NULL)
    {
        
        /* write message and send to server*/
        printf("Enter a message for the server: ");
        
        fgets(finput, BUFFER, stdin); //gives message to server from user input
        sent = send(sock, finput, strlen(finput), 0); //send user input to server
        if (sent < 0 )
        {
            perror("error writing to echo server");
        }
        
        /*read message from server and print out on client side*/
        len = recv(sock, fouput, BUFFER, 0); //read server's reply
        if (len < 0)
        {
            perror("error reading from echo server");
        }
        
        fouput[len] = '\0'; //last byle + null character sent by echo server
  
        printf("%s","The response from the server is: ");
        fputs(fouput, stdout); //print response from echo server

    
    }//end while loop

    printf("Terminating connection to server");
    
    close(sock); //close connection
}
