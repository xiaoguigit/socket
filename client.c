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
	//���������ֺ͵�ַת�����û��������������������Ϊ����
	if(argc<2) {
		printf("You should input IP or Name of the server!/n");
		exit(1);
	}

	he = gethostbyname(argv[1]);
	//����һ��TCP�׽ӿ�
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {
		perror("socket");
		exit(1);
	}

	//��ʼ���ṹ�壬���ӵ���������2323�˿�
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(8888);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(their_addr.sin_zero),8);	

	//�ͷ������������ӣ������ӽ���ʧ����ֱ�ӱ���
	if(connect(sockfd,(struct sockaddr *)&their_addr,
		sizeof(struct sockaddr))==-1){
		perror("connect");
		exit(1);
	}


	//������������ַ���msg
	if(send(sockfd,msg,strlen(msg),0)==-1) {
		perror("send");
		exit(1);
	}

	//���ܴӷ��������ص���Ϣ
	if((numbytes = recv(sockfd,buf,100,0))==-1) {
		perror("recv");
		exit(1);
	}

	buf[numbytes] = '\0';
	printf("result:%s",buf);

	close(sockfd);
	return 0;
}

