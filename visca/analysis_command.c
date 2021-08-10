#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "analysis_command.h"

#define BUFF_SIZE 200


void message_recv_send(int fd)
{
	unsigned char buff[BUFF_SIZE];
	int ret;

     while(1)
    {
        memset(buff,0,sizeof(buff));
        ret = read(fd,buff,BUFF_SIZE);

        if ( ret <= 0 ) 
        {
            printf("client close\n");
            close(fd);
            break;
        }
        else
        {
            if ( ret < BUFF_SIZE )
            {
                int i;
                for(i=0;i<ret;i++)
                {
                    printf("%.2X ",buff[i]);
                }
    
                printf("\n");
            }
			
			printf_command_name(buff);		
            send(fd,"receved data",13,0);
        }
    }

        close(fd);
}


void printf_command_name(char* buff)
{
	//打印出对应命令的名称
	if ( (int)buff[1] == 0x09 )
	{
		printf("Pan-tiltPoslnq:查询位置坐标\n");
	}
	else if ( (int)buff[1] == 0x01 )
	{

		switch ((int)buff[3])
		{
			case 0x02:
				printf("HOME位置\n");
				break;

			case 0x01:
				printf("速度等级%d\n",(int)buff[4]);
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
