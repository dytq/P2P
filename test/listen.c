#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6666
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[BUFFER_SIZE];

    // Création du socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Liaison du socket à l'adresse
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Erreur lors de la liaison du socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Mise en écoute du socket
    if (listen(server_socket, 10) == -1) {
        perror("Erreur lors de la mise en écoute du socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    // Accepter les connexions entrantes
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1) {
            perror("Erreur lors de l'acceptation de la connexion");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        // Adresse IP du client
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Connexion établie avec %s:%d\n", client_ip, ntohs(client_address.sin_port));

        // Gérer la communication avec le client
        // (vous pouvez implémenter ici la logique de votre application P2P)

        // Fermer la connexion avec le client
        close(client_socket);
    }

    // Fermer le socket du serveur (ce code ne sera jamais atteint dans cet exemple infini)
    close(server_socket);

    return 0;
}

