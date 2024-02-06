#ifndef _H_FTP_
#define _H_FTP_

#include <stdio.h>

// Structure de données pour un serveur FTP

typedef struct server_ftp_s {
	char * hostname_main_server;
	char * port_main_server;
	
	char * hostname_local;
	char * port_local;

	// authentification vers le serveur centrale
	// int (*auth)(struct server_ftp_s *, char * login, char * password, int pipefd);

	// envoie des fichiers metadonnées vers le serveur centrale	
	int (*update_metadata) (struct server_ftp_s *, char * file_path);

	void (*open_download) (struct server_ftp_s *);
} server_ftp_t;

// Structure de données pour un client FTP
typedef struct client_ftp_s {
	char * hostname_main_server;
	char * port_main_server;
	
	char * hostname_local;
	char * port_local;

	int * sockfd_listen; 

	// recherche données dans le serveur centrale
	int (*search_key) (struct client_ftp_s *, char * key);
	
} client_ftp_t;

#endif
