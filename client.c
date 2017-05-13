/* client.c */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

static int sockfd,numbytes;
static char recv_buf[1024];
static unsigned char msg_buf[1024];

static int * msg_handle_fun(void *arg);
static int * msg_send_fun(void *arg);


int main(int argc,char *argv[]) 
{
	char *msg="hello world";
	struct hostent *he;
	struct sockaddr_in their_addr;
	int i = 0;
	//将基本名字和地址转换，用户必须输入服务器的名字为参数
	if(argc<2) {
		printf("You should input IP or Name of the server!/n");
		exit(1);
	}

	he = gethostbyname(argv[1]);
	//建立一个TCP套接口
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {
		perror("socket");
		exit(1);
	}

	//初始化结构体，连接到服务器的8888端口
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(8888);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(their_addr.sin_zero),8);	

	//和服务器建立连接，若连接建立失败则直接报错
	if(connect(sockfd,(struct sockaddr *)&their_addr,
		sizeof(struct sockaddr))==-1){
		perror("connect");
		exit(1);
	}

	//创建消息监听线程
	pthread_t msg_handle_thread;
	if(pthread_create(&msg_handle_thread,NULL,(void *)msg_handle_fun,NULL))
	{
		printf("Fail to Create Msg Handle Thread.\n");
		return -1;
	}
	else
	{
		printf("create msg_handle_thread ok.\n");
	}


	//创建消息发送线程
	pthread_t msg_send_thread;
	if(pthread_create(&msg_send_thread,NULL,(void *)msg_send_fun,msg_buf))
	{
		printf("Fail to Create Msg Handle Thread.\n");
		return -1;
	}
	else
	{
		printf("create msg_handle_thread ok.\n");
	}


	strncpy(msg_buf, msg, strlen(msg));

	while(1);

	close(sockfd);
	return 0;
}


static int * msg_handle_fun(void *arg)
{
    while(1) 
	{
		if((numbytes = recv(sockfd, recv_buf, sizeof(recv_buf),0))==-1)
		{
			continue;
		}
		else
		{
			printf("%s\n",recv_buf);
			sleep(5);
		}
	}
}



static int * msg_send_fun(void *arg)
{
	unsigned char *message = (unsigned char *)arg;
    while(1) 
	{
		if(message != NULL)
		{
			if((numbytes = send(sockfd, message, strlen(message),0))==-1)
			{
				continue;
			}
			else
			{
				printf("SEND :%s\n",message);
			}
		}
	}
}


