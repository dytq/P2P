#include "server_ftp.h"
#include "talker.h"

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
			if (nbytes > 0) {
			    printf("SERVER_FTP :\n");
			    printf("hostname %s\n", P.hostname);
			    printf("data %s\n", P.buf);
			    if(data[0] == 'D')
			    {
				    server_ftp->upload(server_ftp, nom_fichier, P.hostname, "6666");
			    }
			    if(data[0] == 'E')
			    {
				server_fpt->write_data(server_ftp, nom_fichier, description_fichier);
				server_ftp->publish_data(server_ftp, nom_fichier, description_fichier);
			    }
			}
		} while (nbytes > 0);

		printf("Finished reading\n");
	}
	return 0;
}

int upload(server_ftp, char * nom_fichier, char * hostname, char * port)
{
	// fonction qui lit un fichier et l'envoie
	// fonction qui envoie End + nom fichier
	return 0;
}

int write_data(server_ftp server, char * nom_fichier, char * description_fichier)
{
	// append nom fichier et description fichier dans le fichier metadata
	return 0;
}	

int publish_data(server_ftp server, char * nom_fichier, char * description_fichier)
{
	// envoie au server central la nouvelle donnée
	return 0;
}
