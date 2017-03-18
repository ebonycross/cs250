//
//  tcpserver.c
//  tcpserver
//
//  Created by ebony cross on 2/20/17.
//  Copyright © 2017 ebony cross. All rights reserved.
//

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



#define BUFFER_SIZE 1024
#define LISTENQ 100
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

//prototypes
void  Listen(int, int);
void sig_chld(int);
void setup(char [], char *[], int *);

int main (int argc, char *argv[]) {
    //if (argc < 1) on_error("Usage: %s [port]\n", argv[0]);
    printf("Welcome to the HTTP Server\n");
    
    int port = 80; //assign server to port #80
    
   // int port = atoi(argv[1]);
    printf("port number is %d \n",port);
    
    //declare variables
    pid_t ChildPid, pid; //create fork to imp multiple processes at one-time
    socklen_t clilen;
    int server_fd, client_fd, err, status;
    struct sockaddr_in servaddr, cliaddr;
    char buf[BUFFER_SIZE];
    char *args[100];
    char ipAddress[INET_ADDRSTRLEN]; //incomming IP-address to server with length

  
    
    
    //1. create TCP socket for server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Could not create socket\n");
    }
    
   
    //2. server's port and Ip address for socket
    bzero(&servaddr, sizeof(servaddr)); //write zeros to byte string
    servaddr.sin_family = AF_INET; //address family
    servaddr.sin_port = htons(port); //16-bit port number if Network Byte order
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP address **inadd_any: specify IP address with connect to any interface
    
    printf("incoming ip address %s\n", ipAddress);
    
    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val); //set options for socket. *Referenced by Professor Pittman
    
    //3. bind server's port to socket
    err = bind(server_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (err < 0) {
        perror("Could not bind socket\n");
    }
    
    //4. listen wrapper function for connection on a socket from client
    Listen(server_fd, LISTENQ); //**LISTENQ: env variable to override the value specified by the caller
    
    if (err < 0) {
        perror("Could not listen on socket\n");
    }
    
    printf("Server is listening on %d\n", port);
    
    
    signal(SIGCHLD, sig_chld); //must call waitpid()
    
    
    
    while (1) {
        clilen = sizeof(cliaddr);
        
        printf("client length is %d\n",clilen);
        
        puts("Waiting for incomming connection...");
        
        
        //5. accept/reject connection sent to server
        client_fd = accept(server_fd, (struct sockaddr *) &cliaddr, &clilen);
        if(client_fd < 0){
            perror("accept error\n");
        }
        
        puts("socket connection has been accepted\n");
        
        inet_ntop(AF_INET, &cliaddr.sin_addr, ipAddress, sizeof(ipAddress)); //convert binary-ip from client to "networkToPresentable" string
        printf("Request from %s:%i\n", ipAddress, ntohs(cliaddr.sin_port));
        
        if (client_fd < 0) {
            perror("Could not establish new connection\n");
        }
        
        ChildPid = fork();
        
        //6. FORKING and EXECVP methods implemented inside child process
        if(ChildPid < 0){
            perror("Error with pid# %d creating new process\n"); //code execute in the parent porcess only if unsuccesful
        }
        
        else  if (ChildPid == 0) {
            close(server_fd); //close listening socket
            printf("Child PID# %d, is handling this process!\n", getpid());
            printf("I am the CHILD with PID# %d and my parent PID# is %d\n", getpid(), getppid());
            
    
            
                int read = recv(client_fd, buf, BUFFER_SIZE, 0);
                
                if (!read) break; // done reading
            
            
                if (read < 0) on_error("Client read failed\n");
                
                printf("Hello Client\nmessage is %s\n", buf);
                printf("size of msg %lu\n", sizeof(buf));
            
               int s = send(client_fd, buf, read, 0);
            
            close(client_fd); //done with this client
            exit(0); //child terminates
        }
        else {
            printf("I am the PARENT with PID# %d and my child is pid # %d\n", getpid(), ChildPid);
            
            
            pid = wait(&status); //pause until a child process exits
            if (pid > 0){
                printf("pid# %d satus# is %d\n", pid, WEXITSTATUS(status));
            }
            
        }
        
        
        close(client_fd); //parent closes socket connection
        
        printf("close parent\n");
        return 0;
        
        
    } //end of outer while loop

} //end of main prgram




/**Listen Function
 -wrapper function for listen function to resolver is the application specifies the backlog
 -it allows some default but alows a command-line or env variable to override default
 **/
void  Listen(int file_descriptor, int backlog)
{
    char *ptr;
    
    /*can ovrride 2nd arrgument with env variable*/
    if((ptr = getenv("LISTENQ")) != NULL){
        backlog = atoi(ptr);
    }
    
    if (listen (file_descriptor, backlog) < 0) {
        perror("listen error");
    }

}
/**Signal Function
 -signal handler function
 -implement wait() to prevent children of fork from becoming zombies
 **/
void sig_chld(int signo){
    pid_t pid;
    int status;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    
    return;

}

void setup(char inputBuffer[], char *args[], int *background) //Ham xu li (cat) chuoi lenh
{
    const char s[4] = " \t\n";
    char *token;
    token = strtok(inputBuffer, s);
    int i = 0;
    while( token != NULL)
    {
        args[i] = token;
        printf("word %d: %s\n", i,  args[i]);
        i++;
        //printf("%s\n", token);
        token = strtok(NULL,s);
    }
    args[i] = NULL;
}




