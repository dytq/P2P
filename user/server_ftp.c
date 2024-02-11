#include "server_ftp.h"
#include "talker.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int update_metadata(server_ftp_t * server_ftp, char * file) 
{
	FILE * F_IN;
	printf("Open file %s\n", file);
	F_IN = fopen(file, "r");
	if(F_IN == NULL)
	{
		fprintf(stderr, "Erreur lecture de fichier: %s", file);
		exit(EXIT_FAILURE);
	}

	char * title = NULL;
	char * desc = NULL;
	size_t len = 0;
	ssize_t title_len;
	ssize_t desc_len;

	while (1) {
		title_len = getline(&title, &len, F_IN);
		if(title_len == -1)
		{
			break; // fin de lecture
		}
		printf("%s",title);
		desc_len = getline(&desc, &len, F_IN);
		if(desc_len == -1)
		{
			fprintf(stderr, "Metadata corrompu");
			break;
		}
		printf("%s",desc);
		// envoie du message
		char buffer[1024];
		memset(buffer,'\0', 1024*sizeof(char));
		strncat(buffer, "P, ", 4);
		strncat(buffer, title, title_len - 1);
		strncat(buffer, ":", 2);
		strncat(buffer, desc, desc_len + 1);
		/*
		if(talker(server_ftp->hostname_main_server, server_ftp->port_main_server, buffer) == -1);
		{
			fprintf(stderr,"Impossible d'envoyer les metadata");
			exit(EXIT_FAILURE);
		}
		*/
		talker(server_ftp->hostname_main_server, server_ftp->port_main_server, buffer);

	}	
	
	if (title != NULL)
	{
        	free(title);
	}
	if (desc != NULL)
	{
		free(desc);
	}
	
	printf("Close File\n");
	fclose(F_IN);
	// free(sockfd);
	return 0;
}

int listen_server_ftp(server_ftp_t * server_ftp, int pipe)
{
	int pid = fork();
	// -1
	if(pid == 0)
	{
		packet_ftp_t P;
		int nbytes;

		do {
			nbytes = read(pipe, &P, sizeof(packet_ftp_t));
			if (nbytes > 0) 
			{
			    //printf("SERVER_FTP :\n");
			    //printf("hostname %s\n", P.hostname);
			    //printf("data %s\n", P.buf);
			    if(strcmp(get_csv_value(P.buf,1), "D") == 0)
			    {
				    server_ftp->upload(server_ftp, get_csv_value(P.buf,2), P.hostname, "6666");
			    }
			    if(strcmp(get_csv_value(P.buf,1),"E") == 0)
			    {
				server_ftp->write_metadata(server_ftp, get_csv_value(P.buf,2), get_csv_value(P.buf,3));
				server_ftp->publish_data(server_ftp, get_csv_value(P.buf,2), get_csv_value(P.buf,3));
				printf("Message recut E\n");
			    }
			    if(strcmp(get_csv_value(P.buf,1),"F") == 0)
			    {
				    printf("Fail to get key\n");
			    }
			}
		} while (nbytes > 0);

		printf("Finished reading\n");
	}
	return 0;
}


int upload(server_ftp_t * server_ftp, char * nom_fichier, char * hostname, char * port)
{
	// envoie fichier ligne par ligne
	{
		// Ouvrir le fichier en mode lecture
		FILE *fichier = fopen(nom_fichier, "r");

		if (fichier == NULL) {
		perror("Erreur lors de l'ouverture du fichier");
		exit(EXIT_FAILURE);
		}

		char ligne[1024];  // Ajustez la taille selon vos besoins
		char buffer[2048]; // Un buffer pour stocker la ligne formatée

		// Lire le fichier ligne par ligne
		while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
			// Formater la ligne selon le format spécifié
			snprintf(buffer, sizeof(buffer), "U,%s,%s", nom_fichier, ligne);

			// Afficher le buffer
			talker(hostname,port,buffer);	
		}

		// Fermer le fichier
		fclose(fichier);
	}

	// envoie de la metadata une fois terminé
	{
		FILE *fichierMetadata = fopen(server_ftp->metadata, "r");

		if (fichierMetadata == NULL) {
			perror("Erreur lors de l'ouverture du fichier metadata.csv");
			exit(EXIT_FAILURE);
		}

		char ligne[1024];  // Ajustez la taille selon vos besoins
		char buffer[2048]; // Un buffer pour stocker la ligne formatée
		
		// Lire le fichier metadata.csv ligne par ligne
		while (fgets(ligne, sizeof(ligne), fichierMetadata) != NULL) {
			// Vérifier si la ligne contient la clé
			if (strstr(ligne, nom_fichier) != NULL) {
				// Formater la ligne selon le format spécifié
				snprintf(buffer, sizeof(buffer), "E,%s,%s", get_csv_value(ligne,1), get_csv_value(ligne,2));
				// Afficher le buffer
				talker(hostname,port,buffer);	
				// Fermer le fichier metadata.csv
				fclose(fichierMetadata);
			        return 0;
			}
		}

		// Fermer le fichier metadata.csv
		fclose(fichierMetadata);
	}
	return 0;
}

int write_metadata(server_ftp_t * server_ftp, char * nom_fichier, char * description_fichier)
{
	FILE *fichier = fopen(server_ftp->metadata, "a");

	if (fichier == NULL) {
	perror("Erreur lors de l'ouverture du fichier");
	exit(EXIT_FAILURE);
	}

	// Formater la ligne selon le format spécifié
	char ligne[1024];  // Ajustez la taille selon vos besoins
	snprintf(ligne, sizeof(ligne), "%s,%s", nom_fichier, description_fichier);

	// Écrire la ligne à la fin du fichier
	fprintf(fichier, "%s\n", ligne);

	// Fermer le fichier
	fclose(fichier);
	return 0;
}	

int publish_data(server_ftp_t * server_ftp, char * nom_fichier, char * description_fichier)
{	
	char buffer[1024];  // Ajustez la taille selon vos besoins
	snprintf(buffer, sizeof(buffer), "%s", nom_fichier);
	talker(server_ftp->hostname_main_server, server_ftp->port_main_server, buffer);
	return 0;
}
