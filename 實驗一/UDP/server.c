#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/times.h>
#include <pthread.h>
#
#define SERVER_PORT 8888
#define BUFFER_SIZE 1500

struct tms time_start, time_end;
clock_t old, new;
int t = 0;
double total_recv_packets = 0;
int Background_TCP_Number = 0;
double ticks;

void* timer(void* arg)
{
	int ex = open("UDP_Receiver.csv", O_CREAT|O_RDWR|O_APPEND, S_IRWXU);
	while(1)
	{
		sleep(1);
		t++;
		if(t == 300)
		{
			if((new = times(&time_end)) == -1)
			{
				printf("time error\n");
				exit(1);
			}
    		break;
		}
	}
	pthread_exit(0);
}

int main(int argc, char* argv[])
{
    int server_fd, ret;
    struct sockaddr_in server_addr, client_addr;
    struct stat sb;
    socklen_t client_length = sizeof(client_addr);
    
    double execute_time;
    pthread_t t1;

    if(argc != 2)
    {
        printf("usage : ./server <BK TCP Number>\n");
        exit(1);
    }
    Background_TCP_Number = atoi(argv[1]);
    int recv_size, send_size;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if(server_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(SERVER_PORT);

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("socket bind fail!\n");
        return -1;
    }
    // receive client message to get client ip and port number
    memset(buffer, '\0', BUFFER_SIZE);
    printf("Waiting for client message\n");
    if ((recv_size = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_length)) < 0){
        printf("Couldn't receive Client data\n");
        return -1;
    }
    printf("recv_size %d\n", recv_size);
    printf("Received message from IP: %s and port: %i\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    printf("Msg from client: %s\n", buffer);
    
    // start to send to client
    printf("Server Start Sending :\n");

    int already_sent = 0;
    int start = 0;            // use to record received packet number
    int fd = open("/home/tony/??????code/??????code/file.txt", O_RDONLY, S_IRWXU);
    int total_send_size = 0;
    int count = 0;
    if(fd == -1)
    {
    	perror("open\n");
    	exit(EXIT_FAILURE);
    }
    if(fstat(fd, &sb) == -1)
    {
    	printf("fstat error\n");
    	exit(1);
    }
    char* file_addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if(file_addr == MAP_FAILED)
    {
    	printf("mmap error\n");
    	exit(1);
    }
    memset(buffer, '\0', BUFFER_SIZE);
    strcpy(buffer, "Server Reply");
    send_size = sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, client_length);
    while(1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        memcpy(buffer, file_addr + total_send_size, BUFFER_SIZE);
        if(start == 0)
        {
            if((old = times(&time_start)) == -1)
            {
                printf("time error\n");
                exit(1);
            }
            if(pthread_create(&t1, NULL, timer, NULL) != 0)
    		{
        		printf("can't create t1 thread\n");
        		exit(1);
    		}
            
        }
        if ((send_size = sendto(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, client_length)) < 0){
            printf("sendto error\n");
            return -1;
        }
        start++;
        usleep(120);
        
        if(t == 300)
        	break;
        total_send_size += send_size;
        if(total_send_size == sb.st_size)
        {
        	total_send_size = 0;
        	count++;
        }
        if(count == 2)
        	break;
    }
    if (pthread_join(t1, NULL) != 0) {
        fprintf(stderr, "Error: pthread_join\n");
        return 1;
    }
    close(server_fd);
    return 0;
}
