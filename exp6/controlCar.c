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

#define ECHO_PORT 2333

struct Payload
{
    char uid;
    short vL1;
    short vR1;
    short vL2;
    short vR2;
    short vL3;
    short vR3;
};

struct msgHdr
{
    char head0;
    char head1;
    char len;
    struct Payload payload;
    char checksum;
};

//char command1={0x09,0x1d,0x0d,0x06,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86};
//char command2={0x09,0x1d,0x0d,0x06,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x86};
    //#��
char    s[] = {0x09,0x1d,0x0d,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06};
    //#��
char    f[] = {0x09,0x1d,0x0d,0x06,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x06};


    //#��
char    b[] = {0x09,0x1d,0x0d,0x06,0xFF,0x80,0xFF,0x80,0xFF,0x80,0xFF,0x80,0xFF,0x80,0xFF,0x80,0x06};

char    l[] = {0x09,0x1d,0x0d,0x06,0xFF,0x80,0x00,0x80,0x00,0x00,0x00,0x00,0xFF,0x80,0x00,0x80,0x06};



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
	struct 	msgHdr forward;
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

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR opening socket");
		return RET_ERR;
	}

#if 0
	/*setsockopt�������ʹ��man setsockopt����鿴��������Ӧ�����д*/
	if((ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(opt))) < 0)
	{
		perror("ERROR setsockopt");
		goto failed;
	}
#endif
	memset(&cliaddr, 0, sizeof(cliaddr));

	/*��д�Է���IP��ַ���˿ںţ��ο�udp_server.c��ʵ�֣�ע��IP��ַ�����ֽ�*/
   	 serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.1.66");//IP address
	//serv_addr.sin_addr.s_addr = host->h_addr;//IP address
	serv_addr.sin_port = htons(ECHO_PORT);   //�˿ں�

	int tcplen = sizeof(struct sockaddr);
	ret = connect(sockfd,(struct sockaddr *)&serv_addr, tcplen);

	memset(&forward,0,sizeof(struct msgHdr));

	forward.head0 = 0x09;
	forward.head1 = 0x1D;
        forward.payload.uid = 0x06;

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
		//num=sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&serv_addr,
		switch(buffer[0])
		{
		    case 'f':
		printf("forward\n");
		num=sendto(sockfd,f,sizeof(f),0,(struct sockaddr*)&serv_addr,
			sizeof(struct sockaddr));
		if(num<=0)
			perror("ERROR sendto");
		else if(num > 0)
			printf("num >0");
		sleep(1);
			break;
		    case 'b':
printf("back\n");
		num=sendto(sockfd,b,sizeof(b),0,(struct sockaddr*)&serv_addr,
			sizeof(struct sockaddr));
		if(num<=0)
			perror("ERROR sendto");
		else if(num > 0)
			printf("num >0");
		


		num=sendto(sockfd,b,sizeof(b),0,(struct sockaddr*)&serv_addr,
			sizeof(struct sockaddr));
		sleep(1);
			break;
		}
	}while(num >= 0);
failed:
	close(sockfd);
	return 0;
}
