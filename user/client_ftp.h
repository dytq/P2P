#include "ftp.h"

int search_key(client_ftp_t *, char *);

int listen_client_ftp(client_ftp_t *, int);

int download(client_ftp_t *, char *, char *, char *);

int write_data(client_ftp_t *, char *, char *);
