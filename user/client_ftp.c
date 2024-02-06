#include "client_ftp.h"
#include "ftp.h"
#include "talker.h"
#include "listener.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


int search_key(client_ftp_t * client_ftp, char * key)
{
	// envoie de la requete au serveur
	talker(client_ftp->hostname_main_server, client_ftp->port_main_server, key);
	
	printf("Attente de la réponse du server...\n");

	// while read 
	 
	// afficher le contenu du pipe 
	// demander si le client doit se connecter au server ftp distant
	// si oui
	//  envoie D, nom fichier
	//  écoute du telechargement jusqu'a la fermeture du socket distant
	return 0;
}
