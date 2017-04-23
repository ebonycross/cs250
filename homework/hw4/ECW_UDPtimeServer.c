/*
 Name: Ebony Cross-Williams
 Course: CS250
 Professor: Dr. Pittman
 Due Date: 04/17/17
 main.c
 timeServer

 Created by ebony cross on 4/16/17.
 Copyright © 2017 ebony cross. All rights reserved
 Example of UDP time server: 
    Write a program for a datagram server  that accepts format strings as input datagrams.
    After the server formats a datae string using the strftime(3) function, it will send the result back to the client program in another datagram.
 
 Work cited from Book: Linux Socket Programming by Example (author Warren W. Gay)
---- Requirements:------
 The outcome of this homework will be a functional time server that responds to client time queries. This will require you to properly implement a simple UDP socket, process queries from a connected client, execute those commands on the local operating systems, and return results to the client.
 
 Be sure to:
 
 
 1.Bind to any IP Address on port 1337/udp
 2.Process queries from the client (you are not required to validate commands) and execute in the operating system. You'll likely need to parse the output so that you only return time to the client
 3. Return the results to the client
 -----------------------
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 *This fn reports the error and 
 * exits back to the shell:
 */
static void bail(const char *on_what)
{
    fputs(strerror(errno), stderr);
    fputs(": ", stderr);
    fputs(on_what, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, const char * argv[]) {
    int binder;
    int z;
    char *srvr_addr = NULL;
    struct sockaddr_in adr_inet; //AF_INET
    struct sockaddr_in adr_clnt; //AF_INET
    int len_inet; //length
    int s; //socket
    char dgram[512]; //recv buffer
    char dtfmt[512]; //date/time result
    time_t td; //current time and date
    struct tm tm; //date time values
    
    /*
     *User a server from the command line, if one has been provided
     *Otherwise, this program will default to using the arbitary address
     *127.0.0.23
     */
    if(argc >= 2){
        //addr on cmdline
        srvr_addr = argv[1];
    }else {
        //use default address
        srvr_addr = "127.0.0.23";
    }
    
    /*
     *Create a UDP socket to use 
     */
    s = socket(AF_INET, SOCK_DGRAM,0);
    if(s < 0){
        bail("socket()");
    }
    
    
    /*
     *Create a socket address for use 
     *with bind(2)
     */
    memset(&adr_inet, 0, sizeof adr_inet);
    adr_inet.sin_family = AF_INET;
    adr_inet.sin_port = htons(1337); //bind on port #1337
    adr_inet.sin_addr.s_addr = inet_addr(srvr_addr);
    
    if(adr_inet.sin_addr.s_addr == INADDR_NONE)
        bail("bad address.");
    
    len_inet = sizeof adr_inet;
    
    /* bind a address to the sock, so thata the client
     * programs can contact the specific server...
     */
    binder = bind(s, (struct sockaddr *)&adr_inet, len_inet);
    if(binder < 0){
        bail("bind()");
    }
    
    /*
     *Now wait for the client requests..."
     */
    for(;;)
    {
        /*block until the porgram receives a 
         *datagram at specified address and port
         */
        len_inet = sizeof adr_clnt;
        z = recvfrom(s, //socket
                     dgram, //receiving buffer
                     sizeof dgram, //max recv buf size
                     0, //Flags: no options
                     (struct sockaddr *)&adr_clnt, //addr
                     &len_inet); //addr len, in and out
        
        if(z < 0 )
            bail("recvfrom(2)");
        
        /*
         *Process the request from the client:
         */
        dgram[z] = 0; //null terminate
        if(!strcasecmp(dgram, "QUIT"))
            break; //quit server
        
        /*
         *get the current date and time
         */
        time(&td); //get current time and date
        tm = *localtime(&td); //get components
        
        /*
         *format a new date and time string, 
         *based upone the input format string from client:
         */
        strftime(dtfmt, //formatted result
                 sizeof dtfmt, //max resut size
                 dgram, //input date/time format
                 &tm); //input date/time values
        
        /*
         *send the formatted result back to the client
         *program
         */
        z = sendto(s, //socket to send result
                   dtfmt, //the datagram result to send
                   strlen(dtfmt), //the datagram length
                   0, //flags: no options
                   (struct sockaddr * )&adr_clnt, //addr
                   len_inet); //client address length
        if(z < 0 )
            bail("sendto(2)");
        
    }//end of for loop
    
    //close the sock and exit
    close(s);
    
    return 0;
    
}//end of program
