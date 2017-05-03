/*
 Name: Ebony Cross-Williams
 Course: CS250
 Instructor: Jason M Pittman
 Due Date: 04/03/2017
 Institute: Capitol Technology University
 Description: Homework #3, create udp echo client
 Filename:ecross-udp-client.c
 Directory: homework/hw1
 
 Created by ebony cross on 4/03/17.
 Copyright © 2017 ebony cross. All rights reserved.
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/errno.h>
#define MAXLINE 1024
#define SERV_PORT 9930

static void recvfrom_alarm(int);
char *sock_ntop(const struct sockaddr *, socklen_t);

//global variables
int daemon_proc; //set nonzero by daemon_int()

/*********************** STANDARD ERR FUNCTIONS *****************************/
static void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
    int errno_save, n;
    char buf[MAXLINE + 1];
    
    errno_save = errno; //value caller might want printed
    
#ifdef HAVE_VSNPRINTF
    vsnprintf(buf, MAXLINE, fmt, ap); //safe
#else
    vsprintf(buf, fmt, ap); //not safe
#endif
    n = strlen(buf);
    if(errnoflag)
        snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
    strcat(buf, "\n");
    
    if (daemon_proc){
        syslog(level, "%s", buf);
        
    }
    else {
        fflush(stdout);
        fputs(buf, stderr);
        fflush(stderr);
    }
    
    return;
}//end of err_doit()


void err_quit(const char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    err_doit(0, LOG_INFO, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}//end of err_quit()

/******************************************************************/


int main (int argc, char ** argv)
{

    int sockfd;
    struct sockaddr_in servaddr;
    int readFr;
    socklen_t sockLen, len;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    int sent;
    struct sockaddr *preply_addr;
    const int on = 1;
    static char *bc_addr = "127.255.255.255";
    int n;
    int len_bc;
    
    if(argc != 3)
        err_quit("ussage: udpcli <IPaddress><Port#>");
    
    //broadcast address
    bc_addr = argv[1];

    
    int port = atoi(argv[2]);
    
    
    
    bzero(&servaddr, sizeof(servaddr)); //write zeros to byte string
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
   //servaddr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
   {
       //create a socket
       perror("error creating socket\n");
       exit(1);
    
   }
    
    printf("connecting on port %d\n", port);
    
    
    //malloc() allocates room for the server's address to be returned by revfrom()
    preply_addr = malloc(sockLen);

    
    ///modify echo client: set version fo broadcast socket option
    //it will print all the replies recieve with 5s
    n = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); //allow multiiple listeners on broadcast address
    if(n < 0)
        perror("setsockopt");
    
    signal(SIGALRM, recvfrom_alarm);
    
    sockLen = sizeof(servaddr);
    
    len_bc = sizeof servaddr;
    
    //bind our socket to the broadcast address
    if(bind(sockfd, (struct sockaddr *)&servaddr, len_bc)<0)
        {
            perror("error with this binding");
        }
    
   
    while(1)
    {
        
        /* write message and send to server*/
        printf("Enter a message for the server: ");
        
        fgets(sendline, MAXLINE, stdin); //read a line from std input
        
        
        sent = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) &servaddr,sockLen); //send line to server using sendto()
        
        printf("sent message:  %s\n", sendline);
        
        if (sent < 0 )
        {
            perror("error writing to echo server\n");
        }
        alarm(5);
        for(;;){
            len = sockLen;
        readFr =  recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *) &preply_addr, &len); //read back the server's echo
        
        if (readFr < 0)
        {
            if(errno == EINTR){
                break;
                perror("error reading from echo server\n");
            }
            else{
                perror("recvfrom error");
            }
        }else{
            printf("receieved message.\n");
            
            recvline[readFr] = '\0'; //null terminate
            
            //inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port),sock_ntop_host(preply_addr, len),
            
            printf("message from %s: %s",sock_ntop(preply_addr, len), recvline);
        }
        
        
       
        fputs(recvline, stdout); //print the echoed line to standard output using fputs
        }//end of for loop
    }//end of while loop
    //free(preply_addr);//free sockaddr
    
    exit(0);
    

}//end of main


static void recvfrom_alarm(int signo){  return; } //just interrupt the recvfrom()


//present IPv4 in format with dotted decimal form or the hex string form of an IPv6 address
char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char portstr[8];
    static char str[128];		/* Unix domain is largest */
    
    switch(sa-> sa_family){
            
        case AF_INET:{
            struct sockaddr_in	*sin = (struct sockaddr_in *) sa;
        
            if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return(NULL);
            
            if(ntohs(sin ->sin_port) !=0){
                snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
                strcat(str, portstr);
            }
            return(str);
        }
    }
}

