#include "server_ftp.h"
#include "talker.h"

#include <stdio.h>
#include <string.h>
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

void open_download(server_ftp_t * server_ftp)
{
	// attendre accept
	// quand accept, on créer une nouvelle conexion dans un fork
	// Upload dans le fork 
}
