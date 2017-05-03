/*
 Name: Ebony Cross-Williams
 Course: CS250
 Instructor: Jason M Pittman
 Due Date: 05/02/2017
 Institute: Capitol Technology University
 Description: Project, create udp echo client with broadcasting
 Filename:ecross_udpser.c
 Directory: project2/ecross_udpser.c
 
 Created by ebony cross on 4/23/17.
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <syslog.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include<netdb.h> //hostent
#define BUFLEN 512
#define MAXFD 64


#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

int daemon_proc;


void err(char *str)
{
    perror(str);
    exit(1);
}



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

//prototypes
int daemon_init(const char *pname, int facility);

int main(int argc, char ** argv)
{
    struct sockaddr_in serv_addr, cli_addr;
    struct sockaddr preply_addr;
    const int on = 1;
    int sockfd, i;
    socklen_t len;
    int len_srvr;
    char sendline[BUFLEN], recvline[BUFLEN];
    int n, z;
    static int so_broadcast = TRUE;
    //static char *sv_addr, *bc_addr;
    static char *sv_addr = "127.0.0.1"; //default broadcast address
    static char *bc_addr = "127.255.255.255"; //default server adddress
    int portno;
    
    printf("%d", argc);
    if(argc == 3){
        bc_addr = argv[1];
    }
    if(argc == 4){
        sv_addr = argv[3];
    }
    if(argc < 2){
        printf("number of args is %d", argc);

        perror("ussage: udpserver <broadcast IPaddress><Port#><server IPAddress>");
        exit(1);
    }
    
    portno = atoi(argv[2]);
    
    daemon_init(argv[0], 0); //run server as daemon
  
    //form the server address
    len_srvr = sizeof serv_addr;
    len_srvr = sizeof cli_addr;

    
    //1.create a udp socket by specifying secnd arg is dgram
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        bail("socket");
    else
        printf("Server : Socket() successful\n");
    
    
    
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); //port from arg line
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IPv4 address for the bind
    
    //allow broadcasts
    z = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof so_broadcast);
    if(z < 0)
        perror("setsockopt");
    
    //2. bind address to socketso clients can listen
    if (bind(sockfd, (struct sockaddr* ) &serv_addr, sizeof(serv_addr)) < 0)
        err("bind");
    else
        printf("Server : bind() successful\n");
    
    
    
    len=sizeof(cli_addr);
    
    while(1)
    {
        printf("SERVER: waiting for data from client\n");
        while(1){
        n = recvfrom(sockfd, recvline, BUFLEN, 0, (struct sockaddr*)&cli_addr, &len);
         if (n==-1)
            perror("recvfrom()");
        printf("Received packet from %s:%d. Message: %s\n\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), recvline);
        
        sendto(sockfd,recvline, n, 0, (struct sockaddr*)&cli_addr, len);
        sleep(4);
    }
       // printf("message sent %s:\n", sendline);
    }
    
    close(sockfd);
    return 0;
}

int daemon_init(const char *pname, int facility)
{
    int i;
    pid_t pid;
    if((pid = fork()) < 0)
    {
        return (-1);
    } else if (pid)
        _exit(0); //parent terminates
    
    //child 1 continues
    if(setsid() <0)
       return (-1);
    
    signal(SIGHUP,SIG_IGN);
    
    if((pid = fork()) <0)
        return(-1);
    else if(pid)
        _exit(0);
    
    //child 2 continues...
    
   daemon_proc = 1; //for err_XXX() fns
    
    chdir("/"); //chnage working directory
    
    //close off file descriptors
    for(i = 0; i < MAXFD; i++){
        close(i);
    }
    
    //redirect the stdin, stdout, and stderr to /dev/null
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    openlog(pname, LOG_PID, facility);
    
    return (0);
}
