#ifndef __CREATE__SERVICE__TCP__
#define __CREATE__SERVICE__TCP__

typedef struct MessageServer
{
	int conn_fd;
	int sock_fd;
}Mes_serv;

Mes_serv* service_init();

#endif
