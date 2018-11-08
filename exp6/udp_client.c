#include <stdio.h>
#include <sys/types.h>//����ϵͳ��������
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#define RET_OK   0
#define RET_ERR -1

#define LISTEN_QUEUE_NUM 5

#define BUFFER_SIZE 256

#define ECHO_PORT 2029

int main(int argc, char **argv)
{
	int sockfd = -1 ;
	int opt = 1;
	struct sockaddr_in cliaddr;
	uint8_t buffer[BUFFER_SIZE];
	int sendbytes = 0;
	int ret = -1,num=-1;
	struct hostent *host;
	struct sockaddr_in serv_addr;
	if(argc < 2){
		fprintf(stderr,"Please enter the server's hostname!\n");
		exit(1);
	}

	/*���������gethostbyname��������Ŀ��������IP ��ַ(�ַ�������) ת��ΪIP��ַ(���ֽڴ�С)*/
	host=gethostbyname(argv[1]);
	//host = 0x11111111;
	
	
	if(/*"gethostbyname fail to run"*/host==NULL)
	{
		/*��ӡʧ����Ϣ*/
		perror("gethostbyname");
		exit(1);
	}

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("ERROR opening socket");
		return RET_ERR;
	}

	/*setsockopt���������ʹ��man setsockopt����鿴��������Ӧ�����д*/
	if((ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(opt))) < 0)
	{
		perror("ERROR setsockopt");
		goto failed;
	}
	memset(&cliaddr, 0, sizeof(cliaddr));

	/*��д�Է���IP��ַ���˿ںţ��ο�udp_server.c��ʵ�֣�ע��IP��ַ�����ֽ�*/
   	 serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//IP address
	serv_addr.sin_port = htons(ECHO_PORT);   //�˿ں�


	
	int len = sizeof(struct sockaddr_in);

	do 
	{
    	printf(" please Enter Message : \n");
		memset(buffer, 0 ,sizeof(buffer));
    	if(fgets(buffer, sizeof(buffer) - 1, stdin) == NULL)
    	{
    		break;
    	}
			
		/*ʹ��sendto�������Ӽ����յ������ݷ��͸��Է�*/
		num=sendto(sockfd,buffer,256,0,(struct sockaddr*)&serv_addr,
			sizeof(struct sockaddr));
if(num<=0)
perror("ERROR sendto");
else if(num > 0)
printf("num >0");


	}while(ret >= 0);
failed:
	close(sockfd);
	return 0;
}