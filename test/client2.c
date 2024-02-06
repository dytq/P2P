#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <unistd.h>

int main(int argc, char * argv[])
{
	struct addrinfo hints;
	struct addrinfo * results;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(argv[1], argv[2], &hints, &results);
	if(ret != 0)
	{
		fprintf(stdout, "erreur get address");
		exit(0);
	}
	
	int sockfd;
	struct addrinfo * r;
	for(r = results; r != NULL; r->ai_next)
	{
		sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if(sockfd == -1)
		{
			continue;
		}
		if(connect(sockfd, r->ai_addr, r->ai_addrlen) != -1) // TCP connexion
			break; 
		close(sockfd);
	}

	if(r == NULL)
	{
		fprintf(stdout, "erreur pas de socket");
	}
	
	freeaddrinfo(results);
	for(int i = 0; i < 10; i++)
	{
		send(sockfd, "test2\n\0", 7, 0);
		sleep(5);
	}
	close(sockfd);
	return 0;
}
