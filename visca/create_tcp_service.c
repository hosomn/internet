#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 200

void service_init()
{
	int sock_fd,conn_fd;
	struct sockaddr_in server_addr;
	char buff[BUFF_SIZE];
	int ret;
	char* ip_addr = "192.168.4.15";
	int port_number = 9999;

	if ( (sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1 )
	{
		fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
		exit(1);
	}

	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	server_addr.sin_port = htons(port_number);

	if ( bind(sock_fd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr)) == -1 )
	{
		fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
		exit(1);
	}
	
	if ( listen(sock_fd,10) == -1 )
	{
		fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
		exit(1);
	}

	while(1)
	{
		if ((conn_fd = accept(sock_fd,(struct sockaddr*)NULL,NULL)) == -1)
		{
			printf("accept socket error: %s\n\a",strerror(errno));
			continue;
		}

		while(1)
		{
			printf("begin read\n");
			memset(buff,0,sizeof(buff));
			ret = recv(conn_fd,buff,BUFF_SIZE,0);
			printf("read over\n");
			if ( ret <= 0 )
			{
				printf("client close\n");
				close(conn_fd);
				break;
			}
			else
			{
				if ( ret < BUFF_SIZE )
				{
					printf("=====\n");
					printf("recv msg from client: %s\n",buff);
				}

				send(conn_fd,"receved data",13,0);
			}
		}
		close(conn_fd);
	}
	close(sock_fd);
	exit(0);

}
