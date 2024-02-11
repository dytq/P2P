#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <unistd.h>
#include "talker.h"

int init_talker(char * addr, char * port)
{
	struct addrinfo hints;
	struct addrinfo * results;

	int sockfd = -1;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(addr, port, &hints, &results);
	if(ret != 0)
	{
		fprintf(stderr, "hostname not found %s:%s\n", addr, port);
		return -1;
	}
	
	struct addrinfo * r;
	for(r = results; r != NULL; r->ai_next)
	{ 
		sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if(sockfd == -1)
		{
			continue;
		}
		if(connect(sockfd, r->ai_addr, r->ai_addrlen) != -1) // TCP conne
		{							    
			break;
		}
		fprintf(stderr, "unreach server\n");
		close(sockfd);
		exit(0);
	}

	if(r == NULL)
	{
		fprintf(stderr, "erreur pas de socket\n");
	}
	
	freeaddrinfo(results);
	return sockfd;
}


int talker(int sockfd, char * text)
{
	send(sockfd, text ,strlen(text) + 1, 0);
}

void close_talker(int sockfd)
{
	close(sockfd);
}
