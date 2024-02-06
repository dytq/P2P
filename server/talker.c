#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <unistd.h>
#include "talker.h"

int talker(char * addr, char * port, char * text, int * sockfd)
{
	if(*sockfd == -1)
	{
		struct addrinfo hints;
		struct addrinfo * results;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		int ret = getaddrinfo(addr, port, &hints, &results);
		if(ret != 0)
		{
			fprintf(stderr, "hostname not found %s:%s\n", addr, port);
			exit(EXIT_FAILURE);
		}
		
		int sockfd2;
		struct addrinfo * r;
		for(r = results; r != NULL; r->ai_next)
		{ 
			*sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
			if(*sockfd == -1)
			{
				continue;
			}
			if(connect(*sockfd, r->ai_addr, r->ai_addrlen) != -1) // TCP conne
			{							    
				break;
			}
			fprintf(stderr, "unreach server\n");
			close(*sockfd);
			exit(0);
		}

		if(r == NULL)
		{
			fprintf(stderr, "erreur pas de socket\n");
		}
		
		freeaddrinfo(results);
	}
	//for(int i = 0; i < 10; i++)
	//{
	printf("%d", *sockfd);
	send(*sockfd, text ,strlen(text) + 1, 0);
	close(*sockfd);
	//	sleep(1);
	//}
	return 0;
}
