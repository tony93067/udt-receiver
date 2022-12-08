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

#define DIE(x) perror(x),exit(1)
#define PORT 12000

char* sys_time;
/***global value***/
int num_client = 1;
int data_size = 0;
double ticks;
int Background_TCP_Number = 0;
int t = 0;
//int cd = 0;
/******************/
void* timer(void* arg)
{
    while(1)
    {
        t++;
        sleep(1);
        //printf("t %d\n", t);
        if(t == 301)
            exit(1);
    }
}

void *send_packet(void *arg)
{
    int fd, ex; // file descriptor
    struct stat sb;
    int j = 0;
    // send function return value
    double send_size = 0;
    // client connection file descriptor
    int cd = *((int *)arg);
    // record total send data size
    long long int total_recvsize = 0;
    pthread_t t1;
    printf("Start Sending Packet!\n");

    // open file
    fd = open("/home/tony/論文code/論文code/實驗一/file.txt", O_RDONLY, S_IRWXU);
    ex = open("TCP_Sender.csv", O_CREAT|O_RDWR|O_APPEND, S_IRWXU);
    
    int len;
    char* file_addr;
    // open file 

    if (fd == -1) {
        perror("open\n");
        exit(EXIT_FAILURE);
    }
    // get file info
    if(fstat(fd, &sb) == -1)
    {
        printf("fstat error\n");
        exit(1);
    }
    
    // map file to memory
    file_addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(file_addr == MAP_FAILED)
    {
        printf("mmap error\n");
        exit(1);
    }
    if(pthread_create(&t1, NULL, timer, NULL) != 0)
    {
        printf("can't create t1 thread\n");
        exit(1);
    }
    if((send_size = send(cd, (char *)file_addr, sb.st_size, 0)) < 0)
    {
        DIE("send");
    }
    
    if(munmap(file_addr, sb.st_size) == -1)
    {
        printf("munmap error\n");
        exit(1);
    }
    close(fd);
    //ticks = sysconf(_SC_CLK_TCK);
    /*************/
    /*char str[100] = {0};
    sprintf(str, "%s\n", "TCP");
    write(ex, str, sizeof(str));
    
    memset(str, '\0', sizeof(str));
    sprintf(str, "%s\t%s\n", "程式執行時間", sys_time);
    write(ex, sys_time, strlen(sys_time));
    
    memset(str, '\0', sizeof(str));
    sprintf(str, "%s\t%d\n", "Background_TCP_Number", Background_TCP_Number);
    write(ex, str, sizeof(str));
    */
    //fprintf(ex, "%s\n", "TCP");
    //fprintf(ex, "%s\t%f\n", "Recv Time", execute_time);
    //close connection
    close(ex);
    close(cd);
    pthread_exit(0);
}

int main(int argc, char **argv)
{
   
    static struct sockaddr_in server;
    int sd,cd; 
    int reuseaddr = 1;
    char buf[256];
    socklen_t len;
    
    int client_len = sizeof(struct sockaddr_in);
    pthread_t p1;
    int ret = 0;
    Background_TCP_Number = atoi(argv[1]);

    if(argc != 3)
    {
        printf("Usage: %s %s %s\n",argv[0], "Background_TCP_Number", "Congestion Control");
        exit(1);
    }

    //open socket
    sd = socket(AF_INET,SOCK_STREAM, 0);
    if(sd < 0)
    {
        DIE("socket");
    }

    /* Initialize address. */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("140.117.171.182");

    //reuse address
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr));

    len = sizeof(buf);
    if(getsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
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
    }

    len = sizeof(buf);

    if(getsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("Change to %s\n", buf);
    //bind
    if(bind(sd,(struct sockaddr *)&server,sizeof(server)) < 0)
    {
        DIE("bind");
    }

    //listen
    if(listen(sd,1) < 0)
    {
        DIE("listen");
    }
    
    
    //num_packets = atoi(argv[1]);
    while(1)
    {
        cd = accept(sd,(struct sockaddr *)&server,&client_len);
        if((ret = pthread_create(&p1, NULL, send_packet, (void*)&cd)) != 0)
        {
            fprintf(stderr, "can't create p1 thread:%s\n", strerror(ret));
            exit(1);
        }
    }
    //close connection
    close(sd);
  
  return 0;
}

