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
#define BUFFER_SIZE 10000

/***global value***/
int num_client = 1;
int data_size = 0;
struct tms time_start,time_end;
clock_t old_time, new_time;
double ticks;
double total_recv_size = 0;


int Background_TCP_Number = 0;
char* cc_method;
int t = 0;
//int cd = 0;
/******************/

void* timer(void* arg)
{
	int ex;
	ex = open("TCP_Receiver.csv", O_CREAT|O_RDWR|O_APPEND, S_IRWXU);
    while(1)
    {
        t++;
        sleep(1);
        if(t == 301)
        {
        	if((new_time = times(&time_end)) == -1)
    		{
        		printf("time error\n");
        		exit(1);
    		}
    		ticks = sysconf(_SC_CLK_TCK);
    		double execute_time = (new_time - old_time)/ticks;
			printf("Execute Time: %2.2f\n", execute_time);

			printf("total recv size %lf \n", total_recv_size);
			/*************/
			char str[100] = {0};
			sprintf(str, "%s\n", "TCP");
			write(ex, str, sizeof(str));
			
			memset(str, '\0', sizeof(str));
    		sprintf(str, "%s\t%d\n", "Background_TCP_Number", Background_TCP_Number);
    		write(ex, str, sizeof(str));
    		
    		
			memset(str, '\0', sizeof(str));
    		sprintf(str, "%s\t%s\n", "Congetion Control", cc_method);
    		write(ex, str, sizeof(str));
    		
    		memset(str, '\0', sizeof(str));
    		sprintf(str, "%s\t%lf\n\n", "Throughput(Mb/s)", (total_recv_size*8/1000000)/execute_time);
    		write(ex, str, sizeof(str));
			pthread_exit(0);
        }
    }
}
void *recv_packet(void *arg)
{
    int fd; // file descriptor
    char buffer[BUFFER_SIZE];
    int j = 0;
    pthread_t t1;
    
    double recv_size = 0;
    // client connection file descriptor
    int cd = *((int *)arg);
    // record total send data size
    num_client++;
    printf("Start Recving Packet!\n");

    // open file
    fd = open("file.txt", O_CREAT|O_RDWR|O_TRUNC, S_IRWXU);
    
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        //recv packet
        if((recv_size = recv(cd, (char *)buffer, BUFFER_SIZE, 0)) < 0)
        {
            DIE("send");
        }
        if(j == 0)
        {
        	if(pthread_create(&t1, NULL, timer, NULL) != 0)
    		{
        		printf("can't create t1 thread\n");
        		exit(1);
    		}
            if((old_time = times(&time_start)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
            j++;
        }
        if(write(fd, buffer, recv_size) < 0)
        {
            printf("write error\n");
            exit(1);
        }
        total_recv_size += recv_size;
        if(t == 301)
        {
        	break;
        }
        /*
        if(recv_size == 0)
        {
            printf("recv finish\n");
            close(fd);
            break;
        }*/
    }
    close(cd);
    close(fd);
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    static struct sockaddr_in server;
    int sd,cd; 
    int reuseaddr = 1;
    int client_len = sizeof(struct sockaddr_in);
    pthread_t p1;
    int ret = 0;
    char buf[256] = {0};
    socklen_t len;
    
    cc_method = (char*)malloc(sizeof(char)*15);
    memset(cc_method, '\0', 15);
    strcpy(cc_method, argv[2]);
    Background_TCP_Number = atoi(argv[1]);

    if(argc != 3)
    {
        printf("Usage: %s %s %s\n",argv[0], "Background_TCP_Number", "congestion control");
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
	printf("Current : %s\n", buf);
	
	if(strcmp(argv[2], "bbr") == 0)
	{
		strcpy(buf, "bbr");
		
    	len = strlen(buf);
		if(setsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
		{
			perror("setsockopt congestion control");
			return -1;
		}
	}
	
	len = sizeof(buf);
	if(getsockopt(sd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
	{
		perror("getsockopt");
		return -1;
	}
	printf("Change to : %s\n", buf);
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
        if((ret = pthread_create(&p1, NULL, recv_packet, (void*)&cd)) != 0)
        {
            fprintf(stderr, "can't create p1 thread:%s\n", strerror(ret));
            exit(1);
        }
    }
    //close connection
    close(sd);
  
  return 0;
}

