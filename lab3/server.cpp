#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <cstring>
#include "itoa.h"
using namespace std;
#define buffer_size 64
int getFileSize(char * fileName)
{
    struct stat st;
    stat(fileName,&st);
    int size = st.st_size;
    printf("filesize = %d\n", size);
    return size;
}
int sendFileSize(int clientSocket,char* fileName)
{   
    string msgFileSize = toString(getFileSize(fileName));
    cout << "file size : " << msgFileSize << endl;
    return write(clientSocket,&msgFileSize[0],buffer_size);    
}
int sendFile(char * fileName,int clientSocket)
{
    FILE *file;
    char tmp[buffer_size];
    int n; 
    
    file = fopen(fileName, "rb");
    if (sendFileSize(clientSocket,fileName) == -1)
    {
        printf("File size writing error!\n");
        exit(1);
    }

    while (1)
    {
        n = fread(tmp, sizeof(char), buffer_size, file);
        if (n <= 0){
            break;
        }
        write(clientSocket,tmp,strlen(tmp));      
    }
    fclose(file);
}
int main(int argc,char * argv[])
{
    char portString[4];
    char fileName[buffer_size];
    FILE *fileCfg;
    char i=0;
    fileCfg = fopen("server.cfg", "r");
    fscanf (fileCfg, "%s",portString);
    fscanf (fileCfg, "%s",fileName);
    int port = atoi(portString);

    printf("port %d file %s\n",port,fileName);

    int clientSocket, listener;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t addr_size = sizeof(struct sockaddr);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        cout << "socket error!\n";
        exit(1);
    }
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "bind error!\n";
        exit(2);
    }

    listen(listener, 1);
    
    while (1)
    {
        clientSocket = accept(listener,(struct sockaddr*)&clientAddr, &addr_size);
        if(clientSocket < 0)
        {
            cout << "accept error!\n";
            exit(3);
        }
        sendFile(fileName,clientSocket);
        close(clientSocket);   
    }

    close(listener);
    printf("End of listen!\n");
    return 0;
}
