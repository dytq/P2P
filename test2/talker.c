#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <unistd.h>
#include "talker.h"

int talker(char * addr, char * port, char * text, struct server_str * server)
{
	if(server->sockfd == -1)
	{
		struct addrinfo hints;
		struct addrinfo * results;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		int ret = getaddrinfo(addr, port, &hints, &results);
		if(ret != 0)
		{
			fprintf(stdout, "erreur get address\n");
			exit(0);
		}
		
		int sockfd2;
		struct addrinfo * r;
		for(r = results; r != NULL; r->ai_next)
		{ 
			server->sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
			if(server->sockfd == -1)
			{
				continue;
			}
			if(connect(server->sockfd, r->ai_addr, r->ai_addrlen) != -1) // TCP conne
			{							    
				break;
			}
			fprintf(stdout, "unreach server\n");
			close(server->sockfd);
			exit(0);
		}

		if(r == NULL)
		{
			fprintf(stdout, "erreur pas de socket\n");
		}
		
		freeaddrinfo(results);
	}
	//for(int i = 0; i < 10; i++)
	//{
	send(server->sockfd, text, strlen(text), 0);
	//close(sockfd);
	//	sleep(1);
	//}
	return 0;
}
