#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <strings.h>
#include <netinet/tcp.h>


#define SERVER_PORT 8888
#define LISTEN_NUM 100
static int server_sockfd;
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;
static int sin_size;
static int new_client_fd;
static int client_num;
static int numbytes;
static int client_set[LISTEN_NUM];
static int * listen_thread_fun(void *arg);
static int * msg_handle_fun(void *arg);
static int * disconnect_handle_fun(void *arg);

static char recv_buf[1024];

int main(int argc, char**argv)
{
    	server_sockfd = socket(AF_INET,  SOCK_STREAM, 0);
    	if( server_sockfd < 0)
    	{
		printf("server socket error.\n");
		return -1;
	}

	server_addr.sin_family		= AF_INET;
	server_addr.sin_port 		= htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero), 0);

	if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) 
	{
		printf("bind error.\n");
		return -1;
	}

	if (listen(server_sockfd, LISTEN_NUM) == -1) {
		printf("listen error.\n");
		return -1;
	}

	//创建监听子线程
	pthread_t connect_listen_thread;
	if(pthread_create(&connect_listen_thread,NULL,(void *)listen_thread_fun,&client_num))
	{
		printf("Fail to Create Listen Thread.\n");
		return -1;
	}
	else
	{
		printf("create connect_listen_thread ok.\n");
	}


	//创建子线程，监听所有的断开
	pthread_t disconnect_handle_thread;
	if(pthread_create(&disconnect_handle_thread,NULL,(void *)disconnect_handle_fun,&client_num))
	{
		printf("Fail to Create Disconnect Handle Thread.\n");
		return -1;
	}
	else
	{
		printf("create disconnect_handle_thread ok.\n");
	}

	//创建消息监听线程
	pthread_t msg_handle_thread;
	if(pthread_create(&msg_handle_thread,NULL,(void *)msg_handle_fun,&client_num))
	{
		printf("Fail to Create Msg Handle Thread.\n");
		return -1;
	}
	else
	{
		printf("create msg_handle_thread ok.\n");
	}

	while(1)
	{
		;
	}
	
}

static int * listen_thread_fun(void *arg)
{
	int *num = (int *)arg;
        while(1) 
	{
		sin_size = sizeof(struct sockaddr_in);
		if ((new_client_fd = accept(server_sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
		{
			//printf("accept error.\n");
			continue;
		}
		else
		{
			client_set[(*num)++] = new_client_fd;
			printf("%s is connected.\n", inet_ntoa(client_addr.sin_addr));
		}
	}
}

static int * msg_handle_fun(void *arg)
{
	int *num = (int *)arg;
	int i = 0;
	int j = 0;
	struct sockaddr_in addr;
	int addrlen = sizeof(struct sockaddr_in);
    while(1) 
	{
		if((*num) == 0)
			continue;
		for(i = 0; i < (*num); i++)
		{
			if((numbytes = recv(client_set[i], recv_buf, sizeof(recv_buf),0))==-1)
			{
				continue;
			}
			else
			{
				recv_buf[numbytes]='\0';
				for(j = 0; j < (*num); j++)
				{
					getpeername(client_set[i], (struct sockaddr * )&addr, &addrlen);
					if(send(client_set[j], recv_buf, sizeof(recv_buf),0)==-1)
					{
						//这里需要处理不能通信的socket
						
						if(client_set[i] < 0)
						{
							if(i != (*num) -1)
							{
								for(j = 0; j < (*num) - 1; j++)
								{
									client_set[j] = client_set[j + 1];
								}
							}
							else
							{
								client_set[i] = 0;
							}
							
							if((*num) > 0)
								(*num)--;
							else
								(*num) = 0;
							printf("%s has already disconnect.\n", inet_ntoa(addr.sin_addr));
						}

						continue;
					}
					else
					{
						printf("send msg to %s \n", inet_ntoa(addr.sin_addr));
					}
				}
			}
		}
	}
}


static int * disconnect_handle_fun(void *arg)
{
	int *num = (int *)arg;
	struct tcp_info info;
	int len = sizeof(struct tcp_info);
	struct sockaddr_in addr;
	int addrlen = sizeof(struct sockaddr_in);

	int i = 0;
	int j = 0;
	
	while(1) 
	{
		
		if((*num) == 0)
			continue;
		for(i = 0; i < (*num); i++)
		{
			getpeername(client_set[i], (struct sockaddr * )&addr, &addrlen);
			if(client_set[i] < 0)
			{
				if(i != (*num) -1)
				{
					for(j = 0; j < (*num) - 1; j++)
					{
						client_set[j] = client_set[j + 1];
					}
				}
				else
				{
					client_set[i] = 0;
				}
				if((*num) > 0)
					(*num)--;
				else
					(*num) = 0;
				printf("%s has already disconnect.\n", inet_ntoa(addr.sin_addr));
			}

			
			getsockopt(client_set[i], IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
			if(info.tcpi_state != 1)
			{
				printf("%s has already disconnect.\n", inet_ntoa(addr.sin_addr));
				if(i != (*num) -1)
				{
					for(j = 0; j < (*num) - 1; j++)
					{
						client_set[j] = client_set[j + 1];
					}
				}
				else
				{
					client_set[i] = 0;
				}
				if((*num) > 0)
					(*num)--;
				else
					(*num) = 0;
			}
		}
	}
}

