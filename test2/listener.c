#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include <string.h>
#include <netdb.h>
#include <sys/types.h>

#include "listener.h"

#define MAX_CLIENT 5

int listener(char * addr, char * port, int pipfd)
{
	struct addrinfo hints;
	struct addrinfo * results;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo("localhost", "6666", &hints, &results);
	if(ret !=0)
	{
		fprintf(stdout, "error getaddr");	
		exit(0);
	}
	
	int sockfd;
	
	struct addrinfo * r;
	for(r = results; r != NULL; r->ai_next)
	{
		sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (sockfd == -1)
		{
			continue;
		}
		if(bind(sockfd, r->ai_addr, r->ai_addrlen) == 0) // lier le socket
		{
			break; // success
		}
		close(sockfd);
	}
	if(r == NULL)
	{
		fprintf(stdout, "erreur rien n'est trouvé...");
		exit(0);
	}
	
	freeaddrinfo(results);

	listen(sockfd, MAX_CLIENT);
	
	while(1)
	{	
		int clientfd = accept(sockfd, NULL, NULL);
		printf("clientfd: %d\n",clientfd);
		
		if(clientfd < 0)
		{
			fprintf(stdout, "erreur accept");
		}
		// attente du message...
		int pid = fork();

		if(pid == -1)
		{
			fprintf(stdout, "erreur fork");
			exit(0);
		}
		if(pid == 0) // processus du fils
		{
			printf("création du processus fils\n");	
			while(1)
			{
				char buf[1024];
				ssize_t n = recv(clientfd, buf, 1024, 0);
				if(n == 0)
				{
					printf("fin de la recepetion\n");
					break;
				}
				printf("message: %s",buf);
				char buffer[1024] = "lst ";
				strcat(buffer, buf);
				write(pipfd, buffer, 1024);
			}
			printf("fin du processus fils\n");
			exit(0);
		}
		if(pid > 0) // processus recut du père
		{

			close(clientfd);
			//wait(NULL); // mise en attente jusqu'à la fin du processus fils	
			//printf("fin du processus fils %d", pid);
		}

	}	

	return 0;
}

