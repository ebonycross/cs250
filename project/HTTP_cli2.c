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
#include<sys/wait.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>
#include <sys/select.h>
#include<errno.h>


#define ERROR -1
#define BUFFER 1024

//prototypes
int hostname_to_ip(char *, char*);
int max(int, int);
int connect_nonb(int, const SA *, socklen_t, int);

struct hostent *he;

int main(int argc, char ** argv){
    //if (argc < 3) {
      //  perror("Usage: %s [website] [port]\n", argv[0]);
        //exit(1);
    //}

    struct sockaddr_in echoServer; //contains ip and port # of the tcp server
    int sock; //connects to the server
    char message_in[BUFFER]; //user input in buffer
    char message_out[BUFFER]; //output from server
    char buffer[BUFFER];
    int len, sent;
    char * hostname = argv[1];
    int port = atoi(argv[2]);
    char ip_address[100];
    char message[BUFFER];
    int size_read = 0;
    int total_size = 0;
    int fd;
    int bytes;
    fd_set rset, wset;
    int sel;
    int maxfdp1, val, stdineof;
    ssize_t n, nwritten;
    char *toiptr, *tooptr, *friptr, *froptr;
    char to[BUFFER], fr[BUFFER];
    
    socklen_t lon;
    
    
    
    hostname_to_ip(hostname, ip_address);//convert hostname -> ip address

    /*create socket for connection*/
    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("Socket error");
        exit(-1);
    }
    
    printf("The socket has been created\n");
    
    //set socket non-blocking
    if(fcntl(sock, F_SETFL, O_NONBLOCK)< 0)
    {
        perror("Error setting socket to non-block state\n");
    }

   
    bzero((char *) &echoServer, sizeof(echoServer));
    bcopy(he->h_addr, &echoServer.sin_addr, he->h_length);
    echoServer.sin_port = htons(port); //port # as 1st argument converted to network byte
    echoServer.sin_family = AF_INET; //set address family
    
    
    printf("hostname %s resolve to ip address %s and port #%d\n", hostname,ip_address, port);

    int opt_val = 1;
    setsockopt(sock, IPPROTO_TCP, SO_REUSEADDR, &opt_val, sizeof opt_val); //set options for socket. *Referenced by Professor Pittman

    if(sock == -1){
        perror("setsockopt");
        exit(1);
    }
    
    int valopt;
    
    printf("...attemptting to connect to socket\n");
    /*create TCP connection to echo server */
   
    if(connect_nonb(sock, (struct sockaddr *)&echoServer, sizeof(struct sockaddr_in), 0) < 0)
    {
        err_sys("connect error");
    }
   /* {
        perror("STATUS"); //failed connection
        
        FD_ZERO(&rset);
        
        FD_SET(sock, &rset);
        if((sel = select((sock +1), &rset, NULL, NULL, NULL) )> 0)
        {
            lon = sizeof(int);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
            if(valopt)
            {
                fprintf(stderr, "ERROR with connection function %d --> %s\n", valopt, strerror(valopt));
                exit(0);
            }
        }
        else
        {
            fprintf(stderr, "TIMEOUT ERROR %d --> %s\n",valopt, strerror(valopt));
            exit(0);
        }//end of inner else condition
        
    }//end of outer IF condition
    else{
        printf("\nSuccessful connection\n");
    }
    
    //set descriptors to nonblocking
    val = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, val | O_NONBLOCK);
    
    val = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);
    
    val = fcntl(STDOUT_FILENO, F_GETFL, 0);
    fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);
    
    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof = 0;
    
    maxfdp1 = max(max(STDIN_FILENO, STDOUT_FILENO), sock) +1;
    
  
    //initialize buffer pointers
      */
    printf("The client is now connected to the server\n");
    
    
    


  

    while(1)
 {
     /*
     
     FD_ZERO(&rset);
     FD_ZERO(&wset);
     
     if(stdineof == 0 && toiptr < &to[BUFFER]) //read from stdin
         FD_SET(STDIN_FILENO, &rset);
     if(friptr < &fr[BUFFER]) //read from socket
         FD_SET(sock, &rset);
     if(tooptr != toiptr) //data to write to socket
         FD_SET(sock, &wset);
     if(fro != friptr) //dat to write to stdout
         FD_SET(STDOUT_FILENO, &wset);
     
     select(maxfdp1, &rset, &wset, NULL, NULL); //wait for one of the 4 conditions to be true
     
     */
     
     
     /* Create file where data will be stored */
     
     fd = open("sample4.html", O_WRONLY | O_CREAT |  O_TRUNC);
     if(fd == -1)
     {
         printf("Error opening file");
         return 1;
     }
     
     
     
   
     
    printf("Enter a request for the HTTP server: \n");
    
   // fgets(message, BUFFER, stdin); //gives message to server from user input
    sent = send(sock, message_out, strlen(message),0); //send user input to server
    
     if (sent < 0 )
     {
         perror("error writing to echo server");
     }
     
    
     printf("message sent is %s\n", message_out);
     bzero(message_out, BUFFER);
     
     int r = recv(sock , message_in, BUFFER,0);
     printf("message recv: %s\n", message_in);
     
     
     
    
 
     printf("STARTING RECV FUNCTION\n");
     
     
     //read file and write file loop
     while(size_read >= 0)
     {
         memset(message ,0 , BUFFER);  //clear the variable
         
         printf("BEFORE: NUMBER OF BYTES READ IS %d\n", size_read);
         if((size_read =  read(sock , message, BUFFER-1) ) > 0)
         {
            
            total_size += size_read;
            message[BUFFER-1] = '\0';
            write(fd,message,size_read);
            printf("message is %s\n", message);
            printf("Total bytes: %d\n", total_size);
            printf("AFTER: NUMBER OF BYTES READ IS %d\n", size_read);
             size_read -= BUFFER;
             
         }
         else
         {
             printf("break\n");
             break;
           
         }
     }//end of while loop
   
    
 }
    
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

int max(int a, int b)
{
    if( a > b)
        return a;
    else if (a < b)
        return b;
    else //a = b ? return either value
        return a;
    
}

int connect_nonb(int sockfd, const SA * saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;
    
    flags = Fcntl(sockfd, F_GETFL, 0);
    Fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)
    
    
    return 0;
}




