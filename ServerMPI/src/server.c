#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SA struct sockaddr // Alias para la estructura sockaddr

int initialize_server() {

    int sockfd;   // Descriptor para el socket del servidor

    struct sockaddr_in servaddr; // Estructura para la dirección del servidor
    

    // Crear el socket del servidor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        // Si ocurre un error al crear el socket, mostrar mensaje y salir
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket creado exitosamente.\n");

    // Configuración del servidor
    bzero(&servaddr, sizeof(servaddr));             // Inicializar la estructura del servidor a cero
    servaddr.sin_family = AF_INET;                  // Familia de direcciones: IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Aceptar conexiones de cualquier interfaz
    servaddr.sin_port = htons(PORT);                // Puerto en el que el servidor escuchará

    // Enlazar el socket a la dirección y puerto especificados
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
        // Si ocurre un error al enlazar, mostrar mensaje y salir
        perror("Error al enlazar el socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Socket enlazado al puerto %d.\n", PORT);

    // Configurar el socket para escuchar conexiones entrantes
    if ((listen(sockfd, 5)) != 0) {
        // Si ocurre un error al configurar el socket para escuchar, mostrar mensaje y salir
        perror("Error al configurar el servidor para escuchar");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Servidor escuchando...\n");


    return sockfd;
}

int accept_client(int server_fd) {
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    int client_fd = accept(server_fd, (struct sockaddr *)&cli, &len);

    if (client_fd < 0) {
        perror("Error al aceptar conexión");
        return -1;
    }

    printf("Conexión aceptada.\n");
    return client_fd;
}

void close_connection(int client_fd) {
    close(client_fd);
    printf("Conexión cerrada.\n");
}
