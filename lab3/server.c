#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define buffer_size 10
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
int sendFile(char * fname,int clientSocket)
{
    FILE *file;
    char tmp[buffer_size];
    int n;
 
    file = fopen(fname, "rb");
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
    if (!argv[1])
    {
        printf("Port is incorrect!\n");
        exit(0);
    }
    int port = getPort(argv[1]);
    int clientSocket, listener;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t addr_size = sizeof(struct sockaddr);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        printf("socket error!\n");
        exit(1);
    }
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("bind error!\n");
        exit(2);
    }

    listen(listener, 1);
    
    while (1)
    {
        clientSocket = accept(listener, &clientAddr, &addr_size);
        if(clientSocket < 0)
        {
            printf("accept error!\n");
            exit(3);
        }
        sendFile("serverFolder/file.txt",clientSocket);
        close(clientSocket);   
    }

    close(listener);
    printf("End of listen!\n");
    return 0;
}