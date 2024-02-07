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

#define MAX_BUFFER_SIZE 256

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


void saisirLoginPassword(char *login, char *password, char * buffer) 
{
	printf("Entrez votre login : ");
	fgets(login, MAX_BUFFER_SIZE, stdin);
	login[strcspn(login, "\n")] = '\0';  // Supprime le caractère de nouvelle ligne

	printf("Entrez votre mot de passe : ");
	fgets(password, MAX_BUFFER_SIZE, stdin);
	password[strcspn(password, "\n")] = '\0';  // Supprime le caractère de nouvelle ligne
	
	// Concaténer login et password dans buffer
    	snprintf(buffer, MAX_BUFFER_SIZE * 2, "%s,%s", login, password);
}

void attend_reponse(char * adresseIP, char * port)
{
    int sockfd = -1;
    sockfd = listener(adresseIP, port, 0);
    listen(sockfd, 10);
    struct sockaddr_in serveurAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);


    // Attente de la connexion d'un client
    int clientSocket = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Erreur lors de l'acceptation de la connexion du client");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connexion acceptée depuis %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Lecture du message du client
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        perror("Erreur lors de la lecture du message du client");
    } else if (bytesRead == 0) {
        printf("La connexion du client est fermée.\n");
    } else {
        // Affichage du message reçu
        buffer[bytesRead] = '\0';
        printf("Message du client : %s\n", buffer);

	if (buffer[0] == 'A') {
            printf("Connexion accepté...\n");
         } 
	 else{
            printf("Connexion non accepté.\n");
            close(clientSocket);
            close(sockfd);
            exit(EXIT_SUCCESS);
	 }
    }

    // Fermeture des sockets
    close(clientSocket);
    close(sockfd);	
}

int main(int argc, char * argv[])
{
	if(argc != 5)
	{
		fprintf(stderr, "Nombre d'arguments non valide:\n- Arg1: adresse hostname local\n- Arg2: port local\n- Arg3: adresse hostname serveur centrale distant\n- Arg4: port distant\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Authentification\n");
	char login[MAX_BUFFER_SIZE];
	char password[MAX_BUFFER_SIZE];
	char buffer[MAX_BUFFER_SIZE * 2];

	saisirLoginPassword(login, password,buffer);

	// Vous pouvez maintenant utiliser les valeurs de login et password comme nécessaire.

	printf("Login saisi : %s\n", login);
	printf("Mot de passe saisi : %s\n", password);

	talker(argv[3],argv[4],buffer);
	
	attend_reponse(argv[1],argv[2]);	

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
	return 0;
}
