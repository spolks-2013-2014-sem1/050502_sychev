#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <stdlib.h>
#include <unistd.h> /* read(), write(), close() */
#include <fcntl.h>  /* open(), O_RDONLY */
#include <sys/stat.h>   /* S_IRUSR */
#include <sys/types.h>  /* mode_t */
#include <cstring>
#include "itoa.h"
#include <fcntl.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
using namespace std;

#define buffer_size 1024
int clientSocket;
int oobSize = 0;
int getPort(char * str)
{
    char* ends;
    const long result = strtol(str, &ends, 10);
    if (ends != strchr(str, '\0')) 
    {   
        printf("Port is incorrect!\n");
        exit(0); 
    }
    return atoi(str);
}
int getServerAddr(char * str)
{
    if (!str)
    {
        printf("Host is incorrect!\n");
        exit(0); 
    }
    return inet_addr(str);   
}
int recvFileSize(int clientSocket)
{
    char tmp[buffer_size];
    recv(clientSocket , tmp , buffer_size , 0);
    return atoi(tmp);
}
int recvFile(int clientSocket,char* fileOut)
{
    //FILE *file2;
    int file2;
    char * tmp = (char*)malloc(buffer_size);
    int n,currentSize = 0;
    int fileSize = recvFileSize(clientSocket);
    printf("File size: %d\n",fileSize);
    int i = 0;
    //file2 = fopen(fileOut, "wb");
    file2 = open(fileOut, O_WRONLY| O_APPEND | O_CREAT, 0755);
    cout << "fcntl " << fcntl(clientSocket, F_SETOWN, getpid()) << endl;
    uint8_t buf[buffer_size];
    int tmpSize = 0;
    while (1)
    {
        n = recv(clientSocket , buf , buffer_size , 0);
        if (n <= 0){
            if (currentSize < fileSize)
            {
                printf("Downloading error! Check your connection! (File fize %d, downloading %d\n",fileSize,currentSize);
                exit(1);
            }
            printf("--debug info-- %d\n",currentSize);
            currentSize = 0;
            break;
        }
        //fwrite(tmp, sizeof(char), strlen(tmp), file2);
        write(file2, buf,  n) ;
        currentSize += n;
    }
    cout << "OOB data size: " << oobSize << endl;
    close(file2);  
}
void signal_handler(int code);
int main(int argc , char *argv[])
{
    signal(SIGURG, signal_handler);
    int port = getPort(argv[2]);
    char * fileName = argv[3];
    if (!argv[3])
    {      
        fileName = "file";
        printf("The file name will be set to the default: %s\n",fileName);
    }
    struct sockaddr_in serverAddr;
    char serversResponse[buffer_size];
     
    clientSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (clientSocket < 0)
    {
        printf("Could not create socket");
    }
     
    serverAddr.sin_addr.s_addr = getServerAddr(argv[1]);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
 
    if (connect(clientSocket , (struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connection...");
    puts("Done!");
    puts("File downloading...");
    recvFile(clientSocket,fileName);  
    puts("Done!");
    close(clientSocket);
    return 0;
}

void signal_handler(int code)
{
    int n = 0;
    char * tmp = (char*)malloc(buffer_size);
    n = recv(clientSocket , tmp , buffer_size , MSG_OOB);
    cout << "n = " << n << endl;
    if (n == -1)
    {
        n++;
        printf("Fail of receiving oob data! (size %d)\n",oobSize);
       // exit(4);        
    }
    oobSize += n;
}