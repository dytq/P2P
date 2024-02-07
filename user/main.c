#include "talker.h"
#include "listener.h"
#include "ftp.h"
#include "server_ftp.h"
#include "client_ftp.h"
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
	if(argc != 5)
	{
		fprintf(stderr, "Nombre d'arguments non valide:\n- Arg1: adresse hostname local\n- Arg2: port local\n- Arg3: adresse hostname serveur centrale distant\n- Arg4: port distant\n");
		exit(EXIT_FAILURE);
	}

	printf("Liaison du port hostname de reception %s:%s\n", argv[1],argv[2]);	
	printf("Démmarage du serveur centrale...\n");
	
	int p_server[2];
	pipe(p_server);
	int p_client[2];
	pipe(p_client);

	int sockfd = -1; // sockfd du listener
	sockfd = listener(argv[1], argv[2], 0);
	listen(sockfd, 5);
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

	int pid = fork();
	packet_ftp_t P;

	if (pid == 0) {
		while (1) 
		{
			int client_socket = accept(sockfd, (struct sockaddr *)&client_address, &client_address_len);
			if (client_socket == -1) 
			{
				perror("Erreur lors de l'acceptation de la connexion");
				exit(EXIT_FAILURE);
			}
			char client_ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
			printf("Connexion établie avec %s:%d\n", client_ip, ntohs(client_address.sin_port));

			// Gérer la communication avec le client
			int pid_client = fork();
			if (pid_client == 0) 
			{
				while (1) 
				{
					ssize_t n = recv(client_socket, P.buf, sizeof(P.buf), 0);
					if (n <= 0) 
					{
						break;
					}

					strncpy(P.hostname, client_ip, INET_ADDRSTRLEN * sizeof(char)); // Copie du hostname
					printf("client ip  %s\n", client_ip);
					write(p_server[1], &P, sizeof(packet_ftp_t));
					write(p_client[1], &P, sizeof(packet_ftp_t));
				}
				// Fermer la connexion avec le client
				close(client_socket);
				exit(0);
			}
			// Fermer la connexion avec le client
			close(client_socket);
		}
		exit(1);
	}

	printf("Initialisation du serveur_ftp...\n");
	/*	
	char inbuf[1024];  	
	int nbytes;
	while ((nbytes = read(p_server[0], inbuf, 1024)) > 0) 
            printf("% s\n", inbuf); 
        if (nbytes != 0) 
            exit(2); 
        printf("Finished reading\n"); 	
	*/

	// Initialisation du serveur_ftp
	server_ftp_t * server_ftp;	
	server_ftp = malloc(sizeof(server_ftp_t));
	if(server_ftp == NULL)
	{
		fprintf(stderr, "Erreur d'allocation de mémoire pour le serveur ftp");
		exit(EXIT_FAILURE);
	}	
	
	server_ftp->update_metadata = update_metadata;	
	server_ftp->listen_server_ftp = listen_server_ftp;
	server_ftp->upload = upload;
	server_ftp->write_metadata = write_metadata;
	server_ftp->publish_data = publish_data;

	server_ftp->hostname_main_server = argv[3];
	server_ftp->port_main_server = argv[4];
	
	server_ftp->metadata = "metadata.csv";	
	server_ftp->listen_server_ftp(server_ftp, p_server[0]);

	printf("Update metadata...\n");
       	server_ftp->update_metadata(server_ftp, server_ftp->metadata);
		
	// création du client_ftp
	client_ftp_t * client_ftp;
	client_ftp = malloc(sizeof(client_ftp_t));
	if(client_ftp == NULL)
	{
		fprintf(stderr, "Erreur d'allocaltion de mémoire pour le client ftp");
		exit(EXIT_FAILURE);
	}

	client_ftp->search_key = search_key;
	client_ftp->listen_client_ftp = listen_client_ftp;
	client_ftp->download = download;
	client_ftp->write_data = write_data;	
	
	client_ftp->hostname_main_server = argv[3];
	client_ftp->port_main_server = argv[4];

	printf("Écoute du client ftp...\n");
	client_ftp->listen_client_ftp(client_ftp, p_client[0]);

	// recherche des mots clées
	while(1)
	{
		printf("cmd P2P > : \n");
		char key[32];
		char * key_ptr = key;
		read_stdin(key_ptr, 32);
		client_ftp->search_key(client_ftp, key);
	}
	/*
	// Init server_ftp
	server_ftp_t * server_ftp;
        server_ftp = malloc(sizeof(server_ftp_t));
	// si null
	// server_ftp->sockfd = -1;

	// Init client_ftp	
	client_ftp_t * client_ftp;
	client_ftp = malloc(sizeof(client_ftp_t));
	// si null
	
	int p[2]; // pipe 2 files descriptor for cmd		
	pipe(p);
	
	int pid_cmd = fork();
	if(pid_cmd == -1)
	{
		fprintf(stderr, "erreur de la création du fork command");
		exit(EXIT_FAILURE);
	}
	if(pid_cmd == 0)
	{
		printf("$> Listening in cmd\n");
		
		command(p[1]);
		
		close(p[1]);
		exit(0);
	}
	if(pid_cmd > 0)
	{
		// void	
	}
	
	//int p_lst[2]; // pipe 2 files descriptor for listener
	// pipe(p_lst);
	
	int pid_lst = fork();
	
	if(pid_lst == -1)
	{
		fprintf(stdout, "erreur fork\n");
		return -1;
	}
	if(pid_lst == 0)
	{
		printf("$> Listening in : %s:%s\n", argv[1],argv[2]); 
		listener(argv[1], argv[2], p[1]);	
		exit(0);
	}
	if(pid_lst > 0)
	{
		char buf[1024];
		while(read(p[0],buf,1024)>0)
		{ 
			exec_cmd(buf, server_ftp, client_ftp);
		}
	}
	*/	
	return 0;
}
