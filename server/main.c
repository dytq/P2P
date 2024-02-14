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

#define MAX_CLIENTS 10
#define MAX_DATA 10

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


struct connexion_s connexion[MAX_CLIENTS];

struct data_s {
	char hostname[20];
	char file_name[524];
	char desc[524];
} data_t;

struct data_s data[MAX_DATA];

struct client_s * create_client(char * uname, char * passwd)
{
	struct client_s * client = malloc(sizeof(client_t));
	strncpy(client->uname, uname, sizeof(uname));
    	strncpy(client->passwd, passwd, sizeof(passwd));
	return client;
}

int auth(char * hostname, char * uname, char * passwd)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(connexion[i].client != NULL)
		{
			if(strcmp(connexion[i].client->uname,uname) == 0)
			{
				if(strncmp(connexion[i].client->passwd, passwd, sizeof(connexion[i].client->passwd)) == 0)
				{
					connexion[i].hostname = hostname;
					return 1;
				}
			}
		}
	}
	return 0;
}

void update_metadata(char * hostname, char * file_name, char * desc)
{
	printf("ajout donnné %s \n", file_name);
	
	for(int i = 0; i < MAX_DATA; i++)
	{
		if(data[i].file_name[0] == '\0')
		{
			strcpy(data[i].hostname, hostname);
			strcpy(data[i].file_name, file_name);
			strcpy(data[i].desc, desc);
			return;
		}
	}
}

char * lookup(char * keys)
{
	for(int i = 0; i < MAX_DATA; i++)
	{
		printf("file name %s\n", data[i].file_name);
		if(strncmp(data[i].file_name, keys, sizeof(data[i].file_name)) == 0)
		{
			return data[i].hostname;
		}
	}
	return NULL;
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
		   printf("authentification de %s\n", get_csv_value(P.buf,2));
		   if(auth(P.hostname, get_csv_value(P.buf,2), get_csv_value(P.buf,3)))
		   {
			   talker(P.hostname, "6666", "A");
		   }
		   else
		   {
			   printf("cannot identifies %s", get_csv_value(P.buf,2));
			   talker(P.hostname, "6666", "F");
		   }
	    }
	    if(strcmp(get_csv_value(P.buf,1),"P") == 0)
	    {
		update_metadata(P.hostname, get_csv_value(P.buf,2), get_csv_value(P.buf,3));    
	    }
	    if(strcmp(get_csv_value(P.buf,1),"L") == 0)
	    {
		char * hostname = lookup(get_csv_value(P.buf,2));		
		if(hostname == NULL)
		{
			talker(P.hostname,"6666", "F");
		}
		else
		{
			char buffer[1024];
			snprintf(buffer, sizeof(buffer),"R,%s,%s",hostname, get_csv_value(P.buf,2));
			printf("response to %s : %s",P.hostname, buffer);
			talker(P.hostname, "6666", buffer);
		}

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

    // remplace by memset
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
	    connexion[i].client = NULL;
    }
    for(int i = 0; i < MAX_DATA; i++)
    {
	    data[i].hostname[0] = '\0';
	    data[i].file_name[0] = '\0';
	    data[i].desc[0] = '\0'; 
    }

    // ajout client
    connexion[0].client = create_client("test","test");
    connexion[0].hostname = NULL;
   	
    connexion[1].client = create_client("bob","bob");
    connexion[1].hostname = NULL;

    connexion[2].client = create_client("alice","alice");
    connexion[2].hostname = NULL;


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

