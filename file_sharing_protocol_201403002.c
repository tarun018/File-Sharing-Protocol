#include "definitions.h"
#include "parser.h"
#include "IndexGet.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "FileDownload.h"
#include "FileHash.h"
#include "udp_server.h"
#include "udp_client.h"

void Print(char *string)
{
	printf("%s\n",string);
	return;
}

int main(int argc,char *argv[])
{
	if(argc != 5) {
		printf("\nRun as: %s <ip_address of server> <listenportno> <connectportno> <protocol>\n",argv[0]);
		fflush(stdout);
		return -1;
	}
	//variables to retrieve
	char *listen;
	 char *portno_listen = argv[2];
	char *ip_address = argv[1];
	  char *portno_connect = argv[3];
	char *proto = argv[4];
	
	//setting which client and which server
	if( strcmp(argv[3], "udp") == 0 )
		is_tcp_client = 0;
	else
		is_tcp_client = 1;
	
	//parent0-child
	pid_t pid;
		pid = fork();
	strcpy(variable,argv[4]);

	//parent
	if(pid > 0)
	{        
		if(is_tcp_client == 0) 	{udp_client(ip_address, portno_connect);}
		else 					{tcp_client(ip_address, portno_connect);}
	}
	//child
	if(pid == 0)
	{
		if(is_tcp_client == 0) udp_server(portno_listen);
		else
		  tcp_server(portno_listen);
	}
	//parent 
	return 0;
}

