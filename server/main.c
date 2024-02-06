#include "talker.h"
#include "listener.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
/*
int get_buff_array(char * buf[10], char * cmd)
{
	// ten bloc max
	char bloc_str[64]; // command size
	int i = 0;
	int j = 0;
	int k = 0;
	while(1)
	{
		char c = cmd[k];
		bloc_str[i] = c;
		i++;
		if(c == ' ' || c == '\n')
		{
			buf[j] = malloc(sizeof(char)*i);
			strncpy(buf[j], bloc_str, i);
			memset(bloc_str, '\0', sizeof(char)*64);	
			j++;
			i = 0;
		}
		if(c == '\n')
		{
			break;
		}
		k++;
	}
	return j;
}

int exec_cmd(char buf[1024], server_ftp_t * server_ftp, client_ftp_t * client_ftp)
{
	char * buff_array[10];
	int nb_bloc = get_buff_array(buff_array, buf);
	if(strncmp(buff_array[0],"cmd", 3) == 0)
	{
		if(strncmp(buff_array[1], "auth", 4) == 0)
		{
			if(nb_bloc == 4)
			{
				server_ftp->auth(buff_array[2],buff_array[3]);
			} 
			else
			{
				printf("auth take 2 arguments: login password\n");	
			}
		} 
	}
	if(strncmp(buff_array[0], "lst", 3) == 0)
	{
		if(strncmp(buff_array[1], "auth", 4) == 0)
		{
			if(nb_bloc == 3)
			{
				server_ftp->update_metadata();
			}
		}
	}
	return 0;
}

int command(int p_cmd)
{
	while(1)
	{
		char buffer[32];
    		char *b = buffer;
    		size_t bufsize = 32;
    		size_t characters;
		characters = getline(&b,&bufsize,stdin);
		char buffer2[32] = "cmd ";
		strcat(buffer2, buffer);
		write(p_cmd, buffer2, 32);
	}
}
*/

void read_stdin(char * str_ptr, int len)
{
	char c = 0;
	int i = 0;
	while(i < len)
	{
		read(STDIN_FILENO, &c, 1);
		if(c == '\n')
		{
			str_ptr[i] = '\0';
			break;
		}
		str_ptr[i] = c;
		i++;
	}
}
int main(int argc, char * argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Nombre d'arguments non valide:\n- Arg1: adresse hostname local\n- Arg2: port local\n- Arg3: adresse hostname serveur centrale distant\n- Arg4: port distant\n");
		exit(EXIT_FAILURE);
	}

	printf("Liaison du port hostname de reception %s:%s\n", argv[1],argv[2]);	
	printf("Démmarage du serveur centrale...\n");
	
	int sockfd = -1; // sockfd du listener
		
	sockfd = listener(argv[1], argv[2], 0);
	
	listen(sockfd, 5);
    	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
while (1) {
        int client_socket = accept(sockfd, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1) {
            perror("Erreur lors de l'acceptation de la connexion");
            // close(server_socket);
            exit(EXIT_FAILURE);
        }

        // Adresse IP du client
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Connexion établie avec %s:%d\n", client_ip, ntohs(client_address.sin_port));

        // Gérer la communication avec le client
        // (vous pouvez implémenter ici la logique de votre application P2P)

        // Fermer la connexion avec le client
        close(client_socket);
    }

	/*
	while(1)
	{
		printf("Attente de clients...\n");
		struct sockaddr * sockaddr_ptr;
		// struct socklen_t * socklen_ptr;
		int clientfd = accept(sockfd, sockaddr_ptr, NULL);
	


		struct addrinfo hints, *info, *p;
		int gai_result;

		char hostname[1024];
		hostname[1023] = '\0';
		gethostname(hostname, 1023);

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_CANONNAME;

		if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
		    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
		    exit(1);
		}

		for(p = info; p != NULL; p = p->ai_next) {
		    printf("hostname: %s\n", p->ai_canonname);
		}

		freeaddrinfo(info);

		if(clientfd < 0)
		{
			fprintf(stderr, "erreur accept clienfd\n");
		}
		printf("création d'un nouveau client %d\n", clientfd);
	}
	*/
	/*
	int p[2];
	pipe(p);
	
	int f = fork();	
	// f == -1 
	if(f == 0)
	{
				exit(0);
	}
	printf("Lecture des données...\n");
	while(1)
	{
		char buf[1024];
		while(read(p[0],buf,1024)>0)
		{ 
			printf("pipe read %s\n", buf);
			break;
		}
	}
	printf("Fin de lecture");
	*/
	return 0;
}
