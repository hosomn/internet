#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLI_NUM 1024
#define BUFF_SIZE   1024



void* func(void* arg)
{
    pthread_detach(pthread_self());

    int ret;
    int fd = *((int*)arg);
    char buff[BUFF_SIZE]={0};
    ret = recv(fd,buff,sizeof(buff),0);

    for (int i=0;i <ret;i++)
    {
        printf("%2.X ",buff[i]);
    }
    printf("\n");
}

int tcp_service_init(unsigned short port_num,const char *ip_addr)
{
    int sock_fd;
    struct sockadd_in server_addr;

    if ( (sock_fd = socker(AF_INET,SOCK_STREAM,0)) == -1 )
    {
        fprintf(stderr,"Socker error:%s\n\a",strerror(errno));
        exit(1);
    }

    bzero(&server_addr,sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    server_addr.sin_port = htons(port_num);

    if ( bind(sock_fd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr)) == -1 )
    {
        fprintf(strerr,"Bind error:%s\n\a",strerror(errno));
        exit(1);
    }

    return sock_fd;
}

int select_service_init(unsigned short port_num,const char *ip_addr)
{
    int serv_fd = tcp_service_init(port_num,ip_addr);
    int cli_fd[MAX_CLI_NUM];
    int cli_num = 0;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(serv_fd,&rfds);
    
    while(1)
    {
        if ( select(cli_num+1,&rfds,NULL,NULL,NULL) <= 0)
        {
            sprintf(strerr,"Select errno:%s\n\a",strerror(errno));
            continue;
        }

        //服务端就绪
        if ( FD_ISSET(sock_fd,&rfds) )
        {
            cli_fd[cli_num] = accept(sock_fd,NULL,NULL);
            if ( cli_fd[cli_num] == -1 )
            {
                sprintf (strerr,"Accept socket error:%s\n\a",strerror(errno));
                continue;
            }
            cli_num++;

            if ( cli_num < MAX_CLI_NUM )
            {
                FD_SET(cli_fd[cli_num-1],&rfds);
            }

        }

        //客户端就绪
        for (int i=0;i<cli_num;i++)
        {
            if ( FD_ISSET(cli_fd[i]) )
            {
                pthread_t ptr_t;
                int ret = pthread_create(&ptr_t,NULL,fun,(void*)cli_fd[i]);
                if ( ret != 0 )
                {
                    printf("create pthread failed\n");
                    continue;
                }
            }
            else
            {
                continue;
            }

        }

    }
        



}

int main(void)
{
    
    return 0;
}