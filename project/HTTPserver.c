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
#include<sys/stat.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

#define BUFFER_SIZE 6024
#define LISTENQ 100
#define MAX_CONN 100
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

//prototypes
void  Listen(int, int);
void sig_chld(int);
void setup(char [], char *[], int *);
void run_Server(char [],int);

//global variables
int client_fd[MAX_CONN];
char buf[BUFFER_SIZE];
char *args[100];
char *ROOT;
char data_to_send[BUFFER_SIZE];
int nread;
int fd;
char path[99999];
char webpage[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>EBONY's CS250 Project 1</title>\r\n"
"<style> body {background-color: #FFFF00 }</style></head>\r\n"
"<body><center><h1>EBONY's CS250 Project 1</h1><br>\r\n";

int main (int argc, char *argv[])
{
    //if (argc < 1) on_error("Usage: %s [port]\n", argv[0]);
    printf("Welcome to the HTTP Server\n");
    
    int port = 8080; //assign server to port #80
    //printf("port number is %d \n",port);
    
    //declare variables
    pid_t ChildPid;
    int pid; //create fork to imp multiple processes at one-time
    socklen_t clilen;
    int server_fd, err, status;
    struct sockaddr_in servaddr, cliaddr;
    int filedes[3];
    int index = 0; //client fd index #
    int counter = 0; //count the number of clients connected
    pipe(client_fd);
    char ipAddress[INET_ADDRSTRLEN]; //incomming IP-address to server with length
    
    
    //Default Values PATH = ~/ and PORT=8080
    ROOT = getenv("PWD");
    
    printf("ROOT: %s\n", ROOT);
    
    
    //set all elements to -1 to indicate there's no client connected at that index
    for(int i= 0; i < MAX_CONN; i++)
    {
        client_fd[i] = -1;
    }
    
    
    
    //1. create TCP socket for server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Could not create socket\n");
    }
    
    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); //set options for socket. *Referenced by Professor Pittman
    
    
    //2. server's port and Ip address for socket
    bzero(&servaddr, sizeof(servaddr)); //write zeros to byte string
    servaddr.sin_family = AF_INET; //address family
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP address **inadd_any: specify IP address with connect to any interface
    servaddr.sin_port = htons(port); //16-bit port number if Network Byte order

    
    printf("port number is %d \n",port);

    //3. bind server's port to socket
    err = bind(server_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (err < 0)
    {
        perror("Could not bind socket\n");
        close(server_fd);
        exit(1);
    }
    
    //4. listen wrapper function for connection on a socket from client
    Listen(server_fd, LISTENQ); //**LISTENQ: env variable to override the value specified by the caller
   
    clilen = sizeof(cliaddr);
    
    
    signal(SIGCHLD, sig_chld); //must call waitpid()
    
    //accept multiple connections
    while(1)
    {
        printf("Waiting for incoming connection...\n");
        
        //5. accept/reject connection sent to server
        client_fd[index] = accept(server_fd, (struct sockaddr *) &cliaddr, &clilen);
        
        printf("Received client fd\n");
        
        if (client_fd[index] >= 0)
        {
            
            fprintf(stdout, "Accept client %d --> %s\n",index, inet_ntoa(cliaddr.sin_addr));
         //   ChildPid = fork();
            
            //6. FORKING method implemented inside child process
            
            if ((ChildPid = fork())== 0)
            {
                close(server_fd); //close listening socket
                
                // printf("Child PID# %d, is handling this process!\n", getpid());
                printf("I am the CHILD with PID# %d and my parent PID# is %d\n", getpid(), getppid());
                // printf("counter# %d\n", counter);
                
                
                //read in data stream and implment execvp()
                run_Server("CHILD",index);
                
               // close(client_fd[index]); //child terminates
                //printf("closing...\n");
                exit(0);
                
            }
            
            else //PARENT PROCESS
            {
                printf("I am the PARENT with PID# %d and my child is pid # %d\n", getpid(), ChildPid);
                
              
                pid = wait(&status); //pause until a child process exits
                
                if (pid > 0)
                {
                    printf("pid# %d status# is %d\n", pid, WEXITSTATUS(status));
                }
                client_fd[index]= -1;
                close(client_fd[index]); //parent closes socket connection
            }//end of inner if-else statement
            
        }//end of outer if-else statelent
        
        else
        {
            perror("Could not establish new connection\n");
            exit(1);
        }
        
        while (client_fd[index] != -1)
            index = (index+1)%MAX_CONN;
        
       // index++;
        counter++;
        
    }//end of while loop
    
    return 0;
}//end of main program

 

/**run_Server Function
 -run servers read and write methods to implement data sent from the client
 **/
void run_Server(char c[], int n)
{
    printf("%s #%d\n",c,n);
    
    int ch;

    
    memset(buf, 0, sizeof(buf));
    
    int reading = recv(client_fd[n], buf, BUFFER_SIZE, 0);
    
    
    int size = strlen(buf);
    buf[size-1] = '\0';
    
    if (!reading)
    {
        perror("reading error\n");
    }
    
   else  if (reading < 0)
    {
        perror("Client read failed\n");
    }
   else
   {
       printf("message is received: %s\n", buf);
 
       
       //parse through message
       setup(buf,args,0);
       
       if ( strncmp(args[0], "GET\0", 4)==0 )
       {
           if ( strncmp( args[2], "HTTP/1.0", 8)!=0 && strncmp( args[2], "HTTP/1.1", 8)!=0 )
           {
               write(client_fd[n], "HTTP/1.0 400 Bad Request\n", 25);
           }
           else
           {
               
               printf("GOOD REQUEST\n");
               

               if ( strncmp(args[1], "/\0", 2)==0 )
                   args[1] = "/index.html";
               
               int counter = 0;
               char *token = strtok(args[1],"/");
               
               printf("FILENAME: %s\n", token);
               
               strcpy(path, ROOT);
               strcpy(&path[strlen(ROOT)], args[1]);
               printf("file: %s\n", path);
               
               if ( (fd=open(path, O_RDONLY)) != -1)    //FILE FOUND
               {
                   send(client_fd[n], "HTTP/1.0 200 OK\n\n", 17, 0);
                 //  write(client_fd[n],webpage,sizeof(webpage)-1);
                  
                   if((nread= read(fd, data_to_send, BUFFER_SIZE)) > 0)
                   {
                      
                           printf("bytes left are %d\n", nread);
                          // printf("in while loop\n");
                           //printf("line %d: data to send-->\n %s\n", counter, data_to_send);
                       
                           write (client_fd[n], data_to_send, nread);
                      // memset(data_to_send, 0, sizeof(data_to_send));
                            fflush(stdout);

                       counter++;
                   }  //end of while loop
                   
               }
    

               
               else write(client_fd[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
           }
       }
   }
    
    close(client_fd[n]);
    client_fd[n]= -1;

    } //end run_Server()


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
}//end of Listen()


/**Signal Function
 -signal handler function
 -implement wait() to prevent children of fork from becoming zombies
 **/
void sig_chld(int signo){
    pid_t pid;
    int status;
    
    while ((pid= waitpid(-1, &status, WNOHANG)) > 0)
        printf("child %d terminated\n\n", pid);
    
    return;
}//end of sig_child()

/*code algorithm cited by programmer: Ham xu li (cat) chuoi lenh on stackoverflow.com*/
void setup(char inputBuffer[], char *args[], int *background) {
    const char s[4] = " \t\r\n"; //I added the \r character for user hitting enter for input
    char *token;
    token = strtok(inputBuffer, s);
    int i = 0;
    while( token != NULL)
    {
        //printf("token value: %s\n", token);
        args[i] = token;
        printf("word %d: %s\n", i,  args[i]);
        i++;
        token = strtok(NULL,s);
    }
    args[i] = NULL;
}//end of setup()



/*

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
    Listen(server_fd, LISTENQ); //LISTENQ: env variable to override the value specified by the caller
    
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
*/






