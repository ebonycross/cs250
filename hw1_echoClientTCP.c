//
//  echoClientTCP.c
//  ECross_Hw1
//
//  Created by ebony cross on 2/19/17.
//  Copyright © 2017 ebony cross. All rights reserved.
//

#include "echoClientTCP.h"
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
    int len;
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
    
    //connection was successful
    while(1)
    {
        fgets(finput, BUFFER, stdin);
        send(sock, finput, strlen(finput), 0);
        
        len = recv(sock, fouput, BUFFER, 0);
        fouput[len] = '\0'; //last byle + null character sent by echo server
        printf("%s\n", fouput);
    
    }//end while loop
    
    close(sock); //close connection
}
