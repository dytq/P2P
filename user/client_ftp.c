#include "client_ftp.h"
#include "ftp.h"
#include "talker.h"
#include "listener.h"
#include "parser.h"

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
			    //printf("CLIENT FTP:\n");
			    //printf("hostname %s\n", P.hostname);
			    //printf("data %s\n", P.buf);

			    if(strcmp(get_csv_value(P.buf,1),"R") == 0)
			    {
				   client_ftp->download(client_ftp, get_csv_value(P.buf,2), get_csv_value(P.buf,3), "6666");
			    } 
			    if(strcmp(get_csv_value(P.buf,1),"U") == 0)
			    {
				client_ftp->write_data(client_ftp, get_csv_value(P.buf,2), get_csv_value(P.buf,3));
				printf("Message recut U\n");
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
	return 0;
}

int download(client_ftp_t * client_ftp, char * nom_fichier, char * hote_distant, char * port)
{
	// envoie de la requete au serveur
	printf("Download data %s", nom_fichier);
	char buffer[1024];
	memset(buffer, '\0', 1024);
	strncat(buffer,"D,",3);
	strncat(buffer, nom_fichier, sizeof(nom_fichier));
	talker(hote_distant, port, buffer);
	return 0;
}

int write_data(client_ftp_t * client_ftp, char * nom_fichier, char * data)
{
	// Ouvrir le fichier en mode ajout (a+ crée le fichier s'il n'existe pas)
	FILE *fichier = fopen(nom_fichier, "a+");

	if (fichier == NULL) {
		perror("Erreur lors de l'ouverture du fichier");
		exit(EXIT_FAILURE);
	}

	// Écrire la ligne à la fin du fichier
	fprintf(fichier, "%s\n", data);

	// Fermer le fichier
	fclose(fichier);	
	return 0;
}
