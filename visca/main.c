#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "create_tcp_service.h"
#include "analysis_command.h"





int main(int argc,char* argv[])
{
	Mes_serv *mes;

	printf("==============service init============\n");
	mes = service_init();

	printf("==============service over============\n");

	printf("============recv begin==========\n");
	message_recv_send(mes->conn_fd);
	
	printf("============recv end============\n");

	close(mes->sock_fd);
	free(mes);
	return 0;
}
