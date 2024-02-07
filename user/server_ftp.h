#include "ftp.h"

int update_metadata(server_ftp_t *, char * );

int listen_server_ftp(server_ftp_t *, int);


int upload(server_ftp_t *, char * nom_fichier, char * hostname, char * port);

int write_data(server_ftp_t *, char * nom_fichier, char * description_fichier);

int publish_data(server_ftp_t *, char * nom_fichier, char * description_fichier);

