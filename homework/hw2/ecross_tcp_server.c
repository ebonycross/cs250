/*
 Name: Ebony Cross-Williams
 Course: CS250
 Instructor: Jason M Pittman
 Due Date: 03/20/2017
 Institute: Capitol Technology University
 Description: create TCP server that allows multiple connections and implments system calls
 Filenam: ecross_tcp_server.c
 Directory: github.com/ebonycross/cs250/Homework/Hw2
 
 Created by ebony cross on 2/20/17.
 Copyright © 2017 ebony cross. All rights reserved.
 
 server string tokenizer code cited from programmer Ham xu li (cat) chuoi lenh
 on www.stackoverflow.com
*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>



#define BUFFER_SIZE 1024
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

int main (int argc, char *argv[])
{
    //if (argc < 1) on_error("Usage: %s [port]\n", argv[0]);
    printf("Welcome to the Telnet Server\n");
    
    int port = 23; //assign server to port #23
    printf("port number is %d \n",port);
    
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
    
   
    //2. server's port and Ip address for socket
    bzero(&servaddr, sizeof(servaddr)); //write zeros to byte string
    servaddr.sin_family = AF_INET; //address family
    servaddr.sin_port = htons(port); //16-bit port number if Network Byte order
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //IP address **inadd_any: specify IP address with connect to any interface
    
    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val); //set options for socket. *Referenced by Professor Pittman
    
    //3. bind server's port to socket
    err = bind(server_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (err < 0)
    {
        perror("Could not bind socket\n");
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
        
        if (client_fd[index] >= 0)
        {
            ChildPid = fork();
            
            //6. FORKING and EXECVP methods implemented inside child process
            if(ChildPid < 0)
            {
                printf("Error with pid# %d creating new process\n", ChildPid); //code
                exit(1);
            }
            
            else if (ChildPid== 0)
            {
                close(server_fd); //close listening socket
                
               // printf("Child PID# %d, is handling this process!\n", getpid());
                printf("I am the CHILD with PID# %d and my parent PID# is %d\n", getpid(), getppid());
               // printf("counter# %d\n", counter);
                
                //read in data stream and implment execvp()
                run_Server("CHILD",index);
                
                exit(0); //child terminates
                
            }
            
            else //PARENT PROCESS
            {
                printf("I am the PARENT with PID# %d and my child is pid # %d\n", getpid(), ChildPid);
                
                //run_Server("Pa", index);
                pid = wait(&status); //pause until a child process exits
                
                if (pid > 0)
                {
                    printf("pid# %d status# is %d\n", pid, WEXITSTATUS(status));
                }
                
                close(client_fd[index]); //parent closes socket connection
            }//end of inner if-else statement
 
        }//end of outer if-else statelent
        
        else
        {
            perror("Could not establish new connection\n");
            exit(1);
        }
        
        index++;
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
    //printf("INSIDE RUN_SERVER(). Index# %d\n", n);
    int read = recv(client_fd[n], buf, BUFFER_SIZE, 0);
        
        
    int size = strlen(buf);
    buf[size-1] = '\0';
        
    if (!read)
    {
        perror("reading error\n");
    }
        
    if (read < 0)
    {
        perror("Client read failed\n");
    }
        
    printf("message is received: %s\n", buf);
    
    setup(buf,args,0);
    
    //redirect stdout file. Duplicate and assign
    if(dup2( client_fd[n], 1) != STDOUT_FILENO)
    {
        perror("Error duplicating file descriptor 1\n");
    }
    
    //redirect stdout file. Duplicate and assign
    if( dup2(client_fd[n], 2) != STDERR_FILENO)
    {
        perror("Error duplicating file descriptor 2\n");
    }
        
        
    //execute command line arguments
    if(execvp(args[0], args) < 0)
    {
        perror("Exec error");
    }
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
        //printf("word %d: %s\n", i,  args[i]);
        i++;
        token = strtok(NULL,s);
    }
    args[i] = NULL;
}//end of setup()




