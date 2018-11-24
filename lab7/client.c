#include "line_parser.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#define MEGIC "2018"

typedef enum {
	IDLE,
	CONNECTING,
	CONNECTED,
	DOWNLOADING,
} c_state;
	
typedef struct {
	char* server_addr;	// Address of the server as given in the [connect] command. "nil" if not connected to any server
	c_state conn_state;	// Current state of the client. Initially set to IDLE
	char* client_id;	// Client identification given by the server. NULL if not connected to a server.
	int sock_fd;		// The file descriptor used in send/recv
} client_state;

struct addrinfo hints;
cmd_line *parsed;
char command[2048];
client_state* cState;
int clientSocket;
struct addrinfo *servinfo;  // will point to the results
int debugMode=0;

int init();
void finish();
void printD(char* mess)
{
    if(debugMode==1)
        fprintf(stderr, "%s|Log: %s\n", cState->server_addr, mess);
}
int exec(char* cmd, char** args, int args_len);

int main (int argc , char* argv[])
{
    int tmp=init();
    if(tmp!=0)
        return tmp; 
    printf("server:%s>", cState->server_addr);
    fgets(command,sizeof(command),stdin);
    while(strcmp(command,"quit\n")!=0)
    {
        
        parsed=parse_cmd_lines(command);
        exec(parsed->arguments[0],(char**)parsed->arguments,parsed->arg_count);
        printf("server:%s>", cState->server_addr);
        *command='\0';
        fgets(command,sizeof(command),stdin);
        free_cmd_lines(parsed);
     
    }
    return 0;   
}
int conn(char* cmd, char** args, int args_len)
{
    if(cState->conn_state!=IDLE)
        {
            perror("client is not on IDLE mode");
            return -2;
        }
        int status;
        memset(&hints, 0, sizeof hints); // make sure the struct is empty
        hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
        if ((status = getaddrinfo(args[1], MEGIC, &hints, &servinfo)) != 0) 
        {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            return 1;
        }
        clientSocket=socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
        if(clientSocket==-1){
            perror("cannot make socket");
            return -2;
        }
        if(connect(clientSocket,servinfo->ai_addr,servinfo->ai_addrlen)==-1)
        {
            perror("connect didn't succeed");
            return -2;
        }
        char* message = malloc(2048);
        message="hello";
        char* messageReceived = malloc(2048);
        int sent = send(clientSocket, message, strlen(message), 0);
      	if (sent < 0)
      	{
        	perror("send() failed");
         	return(-2);
      	}
      	/*/---- Read the message from the server into the buffer ----/*/
      	int rceived = recv(clientSocket, messageReceived,2048,/* MSG_DONTWAIT*/0);
        if (rceived < 0)
        {
            perror("recv() failed");
            return(-2);
        }
        printD(messageReceived);
        char subbuff[6];
        memcpy(subbuff,messageReceived, 5 );
        subbuff[5] = '\0';
        printf("test: -hello- vs -%s-\n",subbuff);
        if(strcmp(subbuff,"hello")==0)
        {
            printf("%s\n",messageReceived);
            char* serverAddress = malloc(sizeof(args[1]));
            strcpy(serverAddress, args[1]);	
            cState->server_addr=serverAddress;
            cState->conn_state=CONNECTED;
            cState->client_id=1;
            cState->sock_fd=clientSocket;
            return 0;
        }
        printf("-%s-\n",messageReceived);
        perror("the server send  another message instead hello\n");
        return -1;
}
int bye(char* cmd, char** args, int args_len)
{
    if(cState->conn_state!=CONNECTED)
        {
            perror("no client is connected");
            return -2;
        }
        
        /*---- Client wants to disconnect from the server ----*/
        const char* data_to_send = "bye";
        send(clientSocket, data_to_send, strlen(data_to_send), 0);
        char* messageReceived =malloc(2048);
        memset(messageReceived,0,2048);
        int rceived = recv(clientSocket, messageReceived,2048,/* MSG_DONTWAIT*/0);
        if (rceived < 0)
        {
            perror("recv() failed");
            return(-2);
        }
        printD(messageReceived);
        finish();
        close(clientSocket);
        return 0;
}
int ls(char* cmd, char** args, int args_len)
{
    if(cState->conn_state!=CONNECTED)
        {
            perror("no client is connected");
            return -2;
        }
        
        /*---- Client wants to disconnect from the server ----*/
        const char* data_to_send = "ls";
        send(clientSocket, data_to_send, strlen(data_to_send), 0);
        char* messageReceived=malloc(3);
        memset(messageReceived,0,3);
        int rceived = recv(clientSocket, messageReceived,3,/* MSG_DONTWAIT*/0);
        if (rceived < 0)
        {
            perror("recv() failed");
            return(-2);
        }
        messageReceived[2]='\0';
        printD(messageReceived);
        if(strcmp(messageReceived,"ok")==0)
        {
            free(messageReceived);
            messageReceived = malloc(2048);
            memset(messageReceived,0,2048);
            rceived = recv(clientSocket, messageReceived,2048,/* MSG_DONTWAIT*/0);
            if (rceived < 0)
            {
                perror("recv() failed");
                return(-2);
            }
            printD(messageReceived);
            free(messageReceived);
        }
        else
            perror("not ok");
        return 0;
}
int get(char* cmd, char** args, int args_len)
{
        if(cState->conn_state!=CONNECTED)
        {
            perror("no client is connected");
            return -2;
        }
        char* data_to_send = malloc(2048);
        memset(data_to_send,0,2048);
        strcat(data_to_send,cmd);
        strcat(data_to_send," ");
        strcat(data_to_send,args[1]);
        printf("test before sending: -%s-\n",data_to_send);
        send(clientSocket, data_to_send, strlen(data_to_send), 0);
        free(data_to_send);
        char* messageReceived =malloc(12);
        memset(messageReceived,0,12);
        int rceived = recv(clientSocket, messageReceived,11,/* MSG_DONTWAIT*/0);
        if (rceived < 0)
        {
            perror("recv() ok file name failed");
            return(-2);
        }
        messageReceived[11]='\0';
        printD(messageReceived);
        char subbuff[3];
        memcpy(subbuff,messageReceived,2 );
        subbuff[2]='\0';
        printf("test: -ok- vs -%s-\n",subbuff);
        if(strcmp(subbuff,"ok")==0)
        {
            cState->conn_state=DOWNLOADING;
            parsed=parse_cmd_lines(messageReceived);
            char *ptr;
            int fileSize;
            fileSize = strtol(parsed->arguments[1], &ptr, 10);
            printf("test: the file size  in integer is %d\n",fileSize);
            char newfileName[2048];
            strcpy(newfileName,args[1]);
            strcat(newfileName,".tmp");
            printf("-new file name- vs -%s-\n",newfileName);
            FILE* tmpfd;
            tmpfd=fopen(newfileName,"ab+");
            int writen =0;
            int rceived;
            int bufSize=fileSize;
            
            while(writen<fileSize)
            {
                char* filebuf =malloc(2048);
                memset(filebuf,0,2048);
                filebuf[0]='\0';
                rceived = recv(clientSocket, filebuf,2048,/* MSG_DONTWAIT*/0);
                if (rceived < 0)
                {
                perror("recv() ok file name failed");
                return(-2);
                }
                printD(filebuf);
                if(bufSize<2048)
                    writen +=fwrite(filebuf,1,bufSize,tmpfd);
                else
                    writen +=fwrite(filebuf,1,2048,tmpfd);
                free(filebuf);
                bufSize-=2048;
                
            }
            fclose(tmpfd);
            printf("test: done\n");
            send(clientSocket, "done", strlen("done"), 0);
            free(messageReceived);
            messageReceived =malloc(2048);
            memset(messageReceived,0,sizeof(messageReceived));
            rceived = recv(clientSocket, messageReceived,2048,/* MSG_DONTWAIT*/0);
            if (rceived < 0)
            {
                perror("recv() ok file name failed");
                return(-2);
            }
            printD(messageReceived);
            if(strcmp(messageReceived,"ok")==0)
            {
                printf("test: wowwwwwwwwwwww\n");
                rename(newfileName,args[1]);
                cState->conn_state=CONNECTED;
                free(messageReceived);
            }
            else
                printf("test: -ok- vs -%s-\n",messageReceived);
       return 0;          
    }
       return -1;
}    
int exec(char* cmd, char** args, int args_len)
{
    if(strcmp(cmd,"conn")==0)
            conn(cmd,args,args_len);
    if(strcmp(cmd,"bye")==0)
            bye(cmd,args,args_len);
    if(strcmp(cmd,"ls")==0)
            ls(cmd,args,args_len);
    if(strcmp(cmd,"get")==0)
            get(cmd,args,args_len);
    if(strcmp(cmd,"-d")==0)
            debugMode=(debugMode+1)%2;
     return 0;
}

void finish()
{
        cState->server_addr="nil";
	cState->conn_state=IDLE;
	cState->client_id=NULL;
	cState->sock_fd=-1;
        freeaddrinfo(servinfo); // free the linked-list
}
int init()
{
        cState=malloc(sizeof(client_state));
        cState->server_addr="nil";
	cState->conn_state=IDLE;
	cState->client_id=NULL;
	cState->sock_fd=-1;
return 0;
}


