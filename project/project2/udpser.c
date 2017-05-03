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
#define BUFLEN 512
//#define PORT 9930


#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

//extern int mkaddr(void *addr, int *addrlen, char *str_addr, char *protocol);


void err(char *str)
{
    perror(str);
    exit(1);
}

void err_quit(const char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    err_doit(0, LOG_INFO, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}//end of err_quit()


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
    
    if(argc > 2){
        bc_addr = argv[1];
    }
    if(argc > 3){
        sv_addr = argv[3];
    } else{
        err_quit("ussage: udpserver <broadcast IPaddress><Port#><server IPAddress>");
    }
    
    portno = atoi(argv[2]);
  
    //form the server address
    len_srvr = sizeof serv_addr;
   // z = mkaddr(&serv_addr, &l, sv_addr, "udp");
    
    len_srvr = sizeof cli_addr;
   // z = mkaddr(&cli_addr, &l, bc_addr, "udp");
    
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
    z = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &so_broadcast, sizeof so_broadcast);
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
        n = recvfrom(sockfd, recvline, BUFLEN, 0, (struct sockaddr*)&cli_addr, &len);
         if (n==-1)
            perror("recvfrom()");
        printf("Received packet from %s:%d. Message: %s\n\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), recvline);
        
        sendto(sockfd,recvline, n, 0, (struct sockaddr*)&cli_addr, len);
        sleep(4);
       // printf("message sent %s:\n", sendline);
    }
    
    close(sockfd);
    return 0;
}
