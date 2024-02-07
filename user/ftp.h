#ifndef _H_FTP_
#define _H_FTP_

#include <stdio.h>

// packet ftp
typedef struct packet_ftp_s {
	char hostname[128];
	char buf[1024];
} packet_ftp_t;

// Structure de données pour un serveur FTP
typedef struct server_ftp_s {
	char * hostname_main_server;
	char * port_main_server;
	
	char * hostname_local;
	char * port_local;

	// authentification vers le serveur centrale
	// int (*auth)(struct server_ftp_s *, char * login, char * password, int pipefd);

	// envoie des fichiers metadonnées vers le serveur centrale	
	int (*update_metadata) (struct server_ftp_s *, char *);

	void (*open_download) (struct server_ftp_s *);

	// listen to pipe (clients)
	int (*listen_server_ftp) (struct server_ftp_s*, int);
	
	int (*upload) (struct server_ftp_s *, char *, char *, char *);

	int (*write_metadata) (struct server_ftp_s *, char *, char *);

	int (*publish_data) (struct server_ftp_s *, char *, char *);

} server_ftp_t;

// Structure de données pour un client FTP
typedef struct client_ftp_s {
	char * hostname_main_server;
	char * port_main_server;
	
	char * hostname_local;
	char * port_local;

	int * sockfd_listen; 

	// recherche données dans le serveur centrale
	int (*search_key) (struct client_ftp_s *, char *);
	
	// listen to pipe
	int (*listen_client_ftp) (struct client_ftp_s *, int);

	// Download data
	int (*download) (struct client_ftp_s *, char *, char *, char *);

	// write data downloaded
	int (*write_data) (struct client_ftp_s *, char *, char *);
} client_ftp_t;

#endif
