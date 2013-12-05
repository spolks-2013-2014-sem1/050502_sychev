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
#include <unistd.h>
using namespace std;
#define buffer_size 1024
 int mainSize = 0;
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
    return write(clientSocket,&msgFileSize[0],64);    
}
int sendFile(char * fileName,int clientSocket)
{
    int i = 0;
    int file;
    char * tmp = (char*)malloc(buffer_size);
    int n; 
    int oobSize = 0;
    file = open(fileName, O_RDONLY);
    printf("result fcntl %d\n",fcntl(clientSocket,F_SETOWN));
    if (sendFileSize(clientSocket,fileName) == -1)
    {
        printf("File size writing error!\n");
        exit(1);
    }
    uint8_t buf[buffer_size];
    int tmpSize = 0;
    while (1)
    {
        //n = fread(tmp, sizeof(char), buffer_size, file);
        n = read(file, buf, buffer_size);
        if (n <= 0){
            break;
        }
        tmpSize = send(clientSocket,buf,n,0);
        usleep(500000);
        oobSize += send(clientSocket,"X",1,MSG_OOB);
        mainSize += tmpSize;
        cout << "Count of sending bytes: " << mainSize << endl;
    }
    cout << "Oob size :" << oobSize << endl;
    close(file);
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

    int opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
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
