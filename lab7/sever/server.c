#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include "common.h"
#include "line_parser.h"




struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};






/*
typedef enum {
	IDLE,
	CONNECTING,
	CONNECTED,
	DOWNLOADING,
} c_state;
	
typedef struct {
	char* server_addr;	// Address of the server as given in the [connect] command. "nil" if not connected to any server
	c_state conn_state;	// Current state of the client. Initially set to IDLE
	int client_id;	// Client identification given by the server. NULL if not connected to a server.
	int sock_fd;		// The file descriptor used in send/recv
} client_state;
*/
#define MEGIC "2018"
#define BACKLOG 10

struct addrinfo hints;
cmd_line *parsed;
int yes;
char* messageformclient;
char* messagetoclient;
char command[2048];
char s[INET6_ADDRSTRLEN];
client_state* cState;
int clientSocket;
int hostnamelen;
char* hostname;
socklen_t socketSize;
struct addrinfo *servinfo,*p;  // will point to the results
struct sockaddr_storage their_addr; // connector's address information
int clientId;
int serverSocket;
int new_fd;
int init();
void finish();
int exec(char* cmd, char** args, int args_len);

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main (int argc , char* argv[])
{
    int tmp=init();
    if(tmp!=0)
        return tmp;
    for(p=servinfo;p!=NULL;p=p->ai_next)
    {
        serverSocket=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if(clientSocket==-1){
            perror("cannot make socket");
            continue;
        }
        if(setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
        {
            perror("setsockopt() failed");
            exit(1);
            
        }
        if(bind(serverSocket,p->ai_addr,p->ai_addrlen)==-1)
        {
            close(serverSocket);
            perror("server binding failed");
            continue;
        }
        break;
    }
    finish();
    
    if(p==NULL)
    {
        perror("server failed to bind");
        exit(1);
    }
    if(listen(serverSocket,BACKLOG)==-1)
    {
        perror("listen failed");
        exit(1);
    }
    
    printf("waiting....to mashiah\n");
    while(1)
    {
        socketSize=sizeof(their_addr);
        new_fd=accept(serverSocket,(struct sockaddr*)&their_addr,&socketSize);
        if(new_fd==-1)
        {
            perror("accept failed");
            continue;
        }
        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
        clientId++;
        printf("server: got connection from %s\n", s);
        /*/---- Read the message from the server into the buffer ----/*/
      	 int rceived;
      	 while((rceived = recv(new_fd, messageformclient,2048/* sizeof(messageformclient)*/,0))>=0)
        {
            parsed=parse_cmd_lines(messageformclient);
            //printf("test parsed: file name is -%s-\n",parsed->arguments[1]);
            if(exec(parsed->arguments[0],(char**)parsed->arguments,parsed->arg_count)!=0)
            {
                perror("exec failed\n");
                exit(1);
            }
            free_cmd_lines(parsed);
            free(messageformclient);
            messageformclient=malloc(2048);
            memset(messageformclient,0,2048);
            //messageformclient[0]='\0';
            if(cState->conn_state!=CONNECTED){
                break;
            }
        }
       
 
    }
    return 0;   
}
int hello(char* cmd, char** args, int args_len){
    if(cState->conn_state!=IDLE)
        {
            perror("client is not on CONNECTED mode");
            return -2;
        }
        cState->conn_state=CONNECTED;
        cState->client_id=malloc(50);
        memset(cState->client_id,0,50);
        sprintf(cState->client_id,"%d",clientId);
        printf("client %s say %s\n",cState->client_id,cmd);
        //return 0;
        messagetoclient=malloc(2048);
        memset(messagetoclient,0,2048);
        strcpy(messagetoclient,"hello ");
        strcat(messagetoclient,cState->client_id);
        printf("test: -hello id- vs -%s-\n",messagetoclient);
        int sent = send(new_fd, messagetoclient,sizeof(messagetoclient), 0);
      	if (sent < 0)
      	{
        	perror("send() failed");
         	return(-2);
      	}
      	return 0;
    
}
int bye(char* cmd, char** args, int args_len)
{
    if(cState->conn_state!=CONNECTED)
        {
            perror("no client is connected");
            return -2;
        }
        
       /*---- Client wants to disconnect from the server ----*/
        messagetoclient = "bye";
        cState->conn_state=IDLE;
        send(new_fd, messagetoclient, strlen(messagetoclient), 0);
        //finish();
        close(clientSocket);
        printf("client say %s\n",messagetoclient);
        return 0;
}

int ls(char* cmd, char** args, int args_len)
{
    if(cState->conn_state!=CONNECTED)
        {
            perror("no client is connected");
            return -2;
        }
         messagetoclient = "ok";
         printf("test : the len of ok is %d\n",(int)strlen(messagetoclient));
         send(new_fd, messagetoclient, strlen(messagetoclient), 0);
         //free(messagetoclient);
         //messagetoclient=malloc(2048);
         //memset(messagetoclient,0,strlen(messagetoclient));
         
        messagetoclient = malloc(2048);
        messagetoclient[0]='\0';
        //messagetoclient=""; 
        messagetoclient=list_dir();
        if(messageformclient==NULL)
        {
            perror("messagetoclient is null");
            return 1;
        }
        send(new_fd, messagetoclient, strlen(messagetoclient), 0);
        return 0;
}
int get(char* cmd, char** args, int args_len)
{
        if(cState->conn_state!=CONNECTED)
        {
            perror("no client is connected");
            return -2;
        }
        messagetoclient = malloc(2048);
        memset(messagetoclient,0,2048);
        strcat(messagetoclient,"ok ");
        int fileSize=file_size(args[1]);
        printf("test: %s size is %d\n",args[1],fileSize);
        char fileSizeStr[50];
        sprintf(fileSizeStr,"%08d",fileSize);
        strcat(messagetoclient,fileSizeStr);
        send(new_fd, messagetoclient, strlen(messagetoclient),0);
        free(messagetoclient);
        messagetoclient=malloc(2048);
        memset(messagetoclient,0,2048);
        messagetoclient[0]='\0';
        cState->conn_state=DOWNLOADING;
        int file_fd=open(args[1],O_RDWR);
        sendfile(new_fd,file_fd,0, fileSize);
        free(messageformclient);
        messageformclient=malloc(2048);
        memset(messageformclient,0,2048);
        if(recv(new_fd, messageformclient,2048/* sizeof(messageformclient)*/,0)<0)
        {
            perror("recv() faild");
            return -2;
        }
        if(strcmp(messageformclient,"done")==0)
        {
            printf("test: bravo\n");
            send(new_fd,"ok",sizeof("ok"),0);
        }
        else
            printf ("test: -done- vs -%s-\n",messageformclient);
    
        cState->conn_state=CONNECTED;
        return 0;
}
int exec(char* cmd, char** args, int args_len)
{
    if(strcmp(cmd,"hello")==0)
             hello(cmd,args,args_len);
    if(strcmp(cmd,"bye")==0)
            bye(cmd,args,args_len);
    if(strcmp(cmd,"ls")==0)
            ls(cmd,args,args_len);
    if(strcmp(cmd,"get")==0)
            get(cmd,args,args_len);
    if(strcmp(cmd,"hello")!=0&&strcmp(cmd,"bye")!=0&&strcmp(cmd,"ls")!=0&&strcmp(cmd,"get")!=0)
        printf("badd message\n");
        

    
    free(hostname);
    return 0;
}

void finish()
{
        //cState->server_addr="nil";
	//cState->conn_state=IDLE;
	//cState->client_id=-1;
	//cState->sock_fd=-1;
        freeaddrinfo(servinfo); // free the linked-list
}
int init()
{
        messageformclient=malloc(2048);
        //messagetoclient=malloc(2048);
        //hostname=malloc(2048);
        cState=malloc(sizeof(client_state));
        cState->server_addr=malloc(2048);
        //cState->client_id=malloc(50);
        int tmp=gethostname(cState->server_addr,2048);
        if(tmp==-1)
        {
            perror("gethostname failed");
            return -1;
        }
	cState->conn_state=IDLE;
	cState->client_id="-1";
	cState->sock_fd=-1;
        int status;
        memset(&hints, 0, sizeof hints); // make sure the struct is empty
        hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
        if ((status = getaddrinfo(NULL, MEGIC, &hints, &servinfo)) != 0) 
        {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            return 1;
        }
        clientId=0;
        yes=1;
        return 0;
}
