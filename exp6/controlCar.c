#include <stdio.h>
#include <sys/types.h>//基本系统数据类型
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
#define SERVICE_ADD "192.168.1.66"

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
	//#stop
char    s[] = { 0x09,0x1d,0x0d,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06 };
//#forward
char    f[] = { 0x09,0x1d,0x0d,0x06,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x06 };


//#back
char    b[] = { 0x09,0x1d,0x0d,0x06,0xFF,0x80,0xFF,0x80,0xFF,0x80,0xFF,0x80,0xFF,0x80,0xFF,0x80,0x06 };
//left
char    l[] = { 0x09,0x1d,0x0d,0x06,0xFF,0x80,0x00,0x80,0x00,0x00,0x00,0x00,0xFF,0x80,0x00,0x80,0x06 };



int main(int argc, char **argv)
{
	int sockfd = -1;
	int opt = 1;
	struct sockaddr_in cliaddr;
	uint8_t buffer[BUFFER_SIZE];
	int sendbytes = 0;
	int ret = -1, num = -1;
	struct hostent *host;
	struct sockaddr_in serv_addr;
	struct 	msgHdr forward;
	if (argc < 2) {
		fprintf(stderr, "Please enter the server's hostname!\n");
		exit(1);
	}

	/*在这里借助gethostbyname函数，将目的主机的IP 地址(字符串类型) 转化为IP地址(四字节大小)*/
	host = gethostbyname(argv[1]);
	//host = 0x11111111;


	if (/*"gethostbyname fail to run"*/host == NULL)
	{
		/*打印失败信息*/
		perror("gethostbyname");
		exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR opening socket");
		return RET_ERR;
	}

#if 0
	/*setsockopt会出错，请使用man setsockopt命令查看函数参数应该如何写*/
	if ((ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) < 0)
	{
		perror("ERROR setsockopt");
		goto failed;
	}
#endif
	memset(&cliaddr, 0, sizeof(cliaddr));

	/*填写对方的IP地址及端口号，参考udp_server.c的实现，注意IP地址是四字节*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.1.66");//IP address
	//serv_addr.sin_addr.s_addr = host->h_addr;//IP address
	serv_addr.sin_port = htons(ECHO_PORT);   //端口号

	int tcplen = sizeof(struct sockaddr);
	ret = connect(sockfd, (struct sockaddr *)&serv_addr, tcplen);

	memset(&forward, 0, sizeof(struct msgHdr));

	forward.head0 = 0x09;
	forward.head1 = 0x1D;
	forward.payload.uid = 0x06;

	int len = sizeof(struct sockaddr_in);

	do
	{
		printf(" please Enter Message : \n");
		memset(buffer, 0, sizeof(buffer));
		if (fgets(buffer, sizeof(buffer) - 1, stdin) == NULL)
		{
			break;
		}

		/*使用sendto函数将从键盘收到的数据发送给对方*/
		//num=sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&serv_addr,
		switch (buffer[0])
		{
		case 'f':
			printf("forward\n");
			num = sendto(sockfd, f, sizeof(f), 0, (struct sockaddr*)&serv_addr,
				sizeof(struct sockaddr));
			if (num <= 0)
				perror("ERROR sendto");
			else if (num > 0)
				printf("num >0");
			sleep(1);
			break;
		case 'b':
			printf("back\n");
			num = sendto(sockfd, b, sizeof(b), 0, (struct sockaddr*)&serv_addr,
				sizeof(struct sockaddr));
			if (num <= 0)
				perror("ERROR sendto");
			else if (num > 0)
				printf("num >0");

			num = sendto(sockfd, b, sizeof(b), 0, (struct sockaddr*)&serv_addr,
				sizeof(struct sockaddr));
			sleep(1);
			break;
		}
	} while (num >= 0);
failed:
	close(sockfd);
	return 0;
}
