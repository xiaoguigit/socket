/* client.c */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
int main(int argc,char *argv[]) 
{
	int sockfd,numbytes;
	char buf[100];
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

	//初始化结构体，连接到服务器的2323端口
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


	//向服务器发送字符串msg
	if(send(sockfd,msg,strlen(msg),0)==-1) {
		perror("send");
		exit(1);
	}

	//接受从服务器返回的信息
	if((numbytes = recv(sockfd,buf,100,0))==-1) {
		perror("recv");
		exit(1);
	}

	buf[numbytes] = '\0';
	printf("result:%s",buf);

	close(sockfd);
	return 0;
}

