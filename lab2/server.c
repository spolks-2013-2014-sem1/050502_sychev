#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
int setPort(char * str)
{
    char* ends;
    const long result = strtol(str, &ends, 10);
    if (ends != strchr(str, '\0')) 
    {   
        printf("Port is incorrect!\n");
        exit(0); /* в строке не число */
    }
    return atoi(str);
}
int main(int argc,char * argv[])
{
    if (!argv[1])
    {
        puts("Error! Check the input data!");
        exit(0);
    }
    int port = setPort(argv[1]);    
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
        
        char * clientIpMsg = "Client IP: ";
        write(clientSocket,clientIpMsg,strlen(clientIpMsg));
        write(clientSocket , inet_ntoa(clientAddr.sin_addr) , strlen(inet_ntoa(clientAddr.sin_addr)));
        write(clientSocket,"\n",1);
        close(clientSocket);
    }

    close(listener);
    printf("Successfull!\n");
    return 0;
}