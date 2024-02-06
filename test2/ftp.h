// Structure de données pour un serveur FTP
typedef struct server_ftp_s {
	// authentification vers le serveur centrale
	int (*auth) (char * login, char * password);

	// envoie des fichiers metadonnées vers le serveur centrale	
	int (*update_metadata) (char * file_path, char * hostname);
} server_ftp_t;

int auth(char * login, char * password, server_data_t * server_data)
{
	return 0;
}

int update_metadata(char * file_path, char * hostname; server_data_t server_data)
{
	return 0;
}

// Structure de données pour un client FTP
typedef struct client_ftp_s {
	int sockfd; // file descriptor du socket client
	char * hostname; // hostname du client

	// recherche données dans le serveur centrale
	int (*search_data) (char * key, server_data_t * server_data);
	
	// envoie les données vers le client ftp
	int (*send_data) (char * path, client_data_t * client_data);
} client_ftp_t;

int search_data(char * key)
{
	return 0;
}

int send_data(char * path)
{
	return 0;
}

typedef struct server_data_s
{
	char * addr_server;
	char * port_server;
	int sockfd;
	int server_status;
	char * metadata_file;
} server_data_t;

typedef struct client_data_s
{
	char * addr_client;
	char * port_client;
	char * hostname;
} client_data_s;
