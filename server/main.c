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

#define MAX_CO 10

// packet ftp
typedef struct packet_ftp_s {
    char hostname[128];
    char buf[1024];
} packet_ftp_t;

// Structure pour représenter un client
struct client_s {
    char uname[20];
    char passwd[20];
} client_t;

struct connexion_s {
	struct client_s * client;
	char * hostname;
} connexion_t;

struct connexion_s connexion[MAX_CO];

struct client_s * create_client(char * uname, char * passwd)
{
	struct client_s * client = malloc(sizeof(client_t));
	strncmp(client->uname, uname, sizeof(uname));
    	strncmp(client->passwd, passwd, sizeof(passwd));
	return client;
}

int listen_server_ftp(int pipe)
{
    packet_ftp_t P;
    int nbytes = 0;

    do {
        nbytes = read(pipe, &P, sizeof(packet_ftp_t));
        if (nbytes > 0)
        {
            printf("SERVER_FTP :\n");
            printf("hostname %s\n", P.hostname);
            printf("data %s\n", P.buf);
            if(strcmp(get_csv_value(P.buf,1),"A") == 0)
	    {
		    printf("auth");
	    }
	}
    } while (nbytes > 0);
    printf("Finished reading\n");
    return 0;
}

int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Nombre d'arguments non valide:\n- Arg1: adresse hostname local\n- Arg2: port local\n");
        exit(EXIT_FAILURE);
    }

    connexion[0].client = create_client("test","test");
    connexion[0].hostname = NULL;
    
    printf("Liaison du port hostname de reception %s:%s\n", argv[1],argv[2]);
    printf("Démmarage du serveur centrale...\n");
    
    int p_server[2];
    pipe(p_server);

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
                    write(p_server[1], &P, sizeof(packet_ftp_t));
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
    
    listen_server_ftp(p_server[0]);    
    return 0;
}
