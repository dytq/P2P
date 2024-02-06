#include "client_ftp.h"
#include "ftp.h"
#include "talker.h"
#include "listener.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <string.h>

// écoute le pipe
int listen_client_ftp(client_ftp_t * client_ftp, int pipe)
{
	int pid = fork();
	// -1
	if(pid == 0)
	{
		packet_ftp_t P;
		int nbytes;

		do {
			nbytes = read(pipe, &P, sizeof(packet_ftp_t));
			if (nbytes > 0) {
			    printf("CLIENT FTP:\n");
			    printf("hostname %s\n", P.hostname);
			    printf("data %s\n", P.buf);

			    if(P.buf[0] == 'R')
			    {
				   client_ftp->download(client_ftp,  nom_fichier, hote_distant, "6666");
			    } 
			    if(P.buf[0] == 'U')
			    {
				client_ftp->write_data(client_ftp, nom_fichier, data);
					
			    }
			}
		} while (nbytes > 0);

		printf("Finished reading\n");
	}
	return 0;
}

// cherche le mot clé
int search_key(client_ftp_t * client_ftp, char * key)
{
	// envoie de la requete au serveur
	char buffer[1024];
	memset(buffer, '\0', 1024);
	strncat(buffer,"L,",3);
	strncat(buffer, key, 33);
	talker(client_ftp->hostname_main_server, client_ftp->port_main_server, buffer);
	
	printf("Attente de la réponse du server...\n");

	return 0;
}

int download(client_ftp_t * client_ftp, char * nom_fichier, char * hote_distant, char * port)
{
	// envoie de la requete au serveur
	char buffer[1024];
	memset(buffer, '\0', 1024);
	strncat(buffer,"D,",3);
	strncat(buffer, nom_fichier, sizeof(nom_fichier));
	talker(hote_distant, port, buffer);
	return 0;
}
