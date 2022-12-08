#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/times.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#define BUFFER_SIZE 10000
#define DIE(x) perror(x),exit(1)


int main(int argc, char **argv)
{
    // use to get socket info
    char buf[256];
    socklen_t len;
    
    static struct sockaddr_in server;
    int sd, cd, i;
    int reuseaddr = 1;
    int client_len = sizeof(struct sockaddr_in);
    char* buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);
    int port = atoi(argv[1]);
    
    if(argc != 3)
    {
        printf("Usage: %s <port number> <CC Name>\n",argv[0]);
        exit(1);
    }

    //open socket
    sd = socket(AF_INET,SOCK_STREAM,0);
    //int ret=setsockopt(sd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(sd < 0)
    {
        DIE("socket");
    }

    /* Initialize address. */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("140.117.171.182");
    
    /*if(getsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("Current %s\n", buf);
    
    if(strcmp(argv[2], "bbr") == 0)
    {
        strcpy(buf, "bbr");
        
        if(setsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
        {
            perror("setsockopt");
            return -1;
        }
    }*/
    //reuse address
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr));

    //bind
    if(bind(sd,(struct sockaddr *)&server,sizeof(server)) < 0)
    {
        DIE("bind");
    }

    //listen
    if(listen(sd,15) < 0)
    {
        DIE("listen");
    }
   
    //num_packets = atoi(argv[1]);
    cd = accept(sd,(struct sockaddr *)&server,&client_len);
    printf("connection accept\n");
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        if(recv(cd, buffer, BUFFER_SIZE, 0) < 0)
        {
            printf("%s\n", strerror(errno));
        }
    }
    //close connection
    close(sd);
  
  return 0;
}

