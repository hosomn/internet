#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLI_NUM 1024
#define BUFF_SIZE   1024

int cli_fd[MAX_CLI_NUM] = {0};
int cli_num = 0;
//int lost_cli_num = 0;   
int serv_fd;
fd_set rfds;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void printf_command_name(char* buff)
{
    //打印出对应命令的名称
    if ( (int)buff[1] == 0x09 )
    {   
        printf("Pan-tiltPoslnq:查询位置坐标: ");
    }   
    else if ( (int)buff[1] == 0x01 )
    {   

        switch ((int)buff[3])
        {
            case 0x02:
                printf("HOME位置: ");
                break;

            case 0x01:
                printf("速度等级%d: ",(int)buff[4]);
                break;

            default:
                printf("invalid command\n");
                break;
        }
    }   
    else
    {   
        printf("invalid command\n");
    }   
    
}


void* func(void* arg)
{
    pthread_detach(pthread_self());

    pthread_mutex_lock(&mutex);
    int ret;
    int index = (int)arg;
    int fd = cli_fd[index];
    

    unsigned char buff[BUFF_SIZE]={0};
    ret = recv(fd,buff,sizeof(buff),0);
    
    if ( ret <= 0 )
    {        
        printf("client close\n");
        close(fd);
        //lost_cli_num++;
        FD_CLR(fd,&rfds);
        cli_fd[index] = 0;
    }
    else
    {
        printf_command_name(buff);
        for (int i=0;i <ret;i++)
        {
            printf("%.2X ",buff[i]);
        }
        printf("\n");
    }
    pthread_mutex_unlock(&mutex);
    //free(arg);
    
}


int tcp_service_init(unsigned short port_num,const char *ip_addr)
{
    int sock_fd;
    struct sockaddr_in server_addr;

    if ( (sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1 )
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
        fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
        exit(1);
    }

    if ( listen(sock_fd,1024) == -1 )
    {
        fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
        exit(1);
    }

    return sock_fd;
}


int select_service_init(unsigned short port_num,const char *ip_addr)
{
    serv_fd = tcp_service_init(port_num,ip_addr);
    if ( serv_fd < 0 )
    {
        printf("Tcp service init failed!\n");
        exit(1);
    }

    int retval;
    int max_fd = serv_fd;
    int new_fd;
    struct timeval tv;

    int thread_count = 0;
    printf("port number is:%d,ip address is:%s\n",port_num,ip_addr);  

    while(1)
    {
        pthread_mutex_lock(&mutex);

        FD_ZERO(&rfds);
        FD_SET(serv_fd,&rfds);

        for ( int i = 0;i < cli_num;i++ )
        {
            if ( cli_fd[i] != 0 )
            {
                FD_SET(cli_fd[i],&rfds);
            }
        }
        
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        retval = select(max_fd+1,&rfds,NULL,NULL,&tv);
        if (  retval < 0 )
        {
            fprintf(stderr,"Select errno:%s\n\a",strerror(errno));
            break;
        }
        else if ( retval == 0 )
        {
            printf("Time out\n");
            pthread_mutex_unlock(&mutex);
            continue;
        }

        //服务端就绪
        if ( FD_ISSET(serv_fd,&rfds) )
        {

            new_fd = accept(serv_fd,NULL,NULL);
            if ( new_fd <= 0 )
            {
                fprintf (stderr,"Accept socket error:%s\n\a",strerror(errno));
                continue;
            }
            else
            {
                printf("New client connect succeed!!!\n");
            }


            if ( cli_num < MAX_CLI_NUM )
            {
                for( int i = 0;i < MAX_CLI_NUM;i++)
                {
                    if ( cli_fd[i]  == 0 )
                    {
                        cli_fd[i] = new_fd;
                        break;
                    }
                }

                max_fd = ( max_fd > new_fd?max_fd:new_fd);
                cli_num++;

            }
            else
            {
                printf("Max connections arrive,exit\n");
                send(new_fd,"bye",4,0);
                close(new_fd);
                continue;
            }

        }

        //客户端就绪
        for (int i=0;i<cli_num;i++)
        {
            if ( FD_ISSET(cli_fd[i],&rfds) )
            {
                pthread_t ptr_t;
                //int* client_fd = (int*)malloc(sizeof(int));
                //*client_fd = i;
                
                printf("the new thread %d\n",thread_count++);
                int ret = pthread_create(&ptr_t,NULL,func,(void*)i);
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

        pthread_mutex_unlock(&mutex);
        usleep(500);

    }
}

int main(int argc,char* argv[])
{
    int port = (unsigned short)atoi(argv[1]);
    select_service_init(port,argv[2]);

    pthread_mutex_destroy(&mutex);

    return 0;
}

