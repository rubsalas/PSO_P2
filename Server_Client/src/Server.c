#include <stdio.h>
#include <stdbool.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <ctype.h>  // isalpha()
#define MAX 1024
#define PORT 8080
#define SA struct sockaddr

// Función para contar palabras en un archivo
void analizar_archivo(const char *filename, char *palabra_mas_repetida, int *max_reps) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo para análisis");
        strcpy(palabra_mas_repetida, "Error");
        *max_reps = -1;
        return;
    }

    char buffer[MAX], copia[MAX];
    char *palabras[MAX];
    int conteos[MAX] = {0};
    int n_palabras = 0;

    // Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        strncpy(copia, buffer, sizeof(copia) - 1);
        copia[sizeof(copia) - 1] = '\0';

        // Tokenizar palabras (considerando delimitadores: espacio, nueva línea, tabulación)
        char *token = strtok(copia, " \t\n");
        while (token != NULL) {
            // Limpiar palabras (eliminar caracteres no alfabéticos al inicio o al final)
            int start = 0, end = strlen(token) - 1;
            while (start <= end && !isalpha(token[start])) start++;
            while (end >= start && !isalpha(token[end])) end--;
            token[end + 1] = '\0';
            token += start;

            // Procesar palabra
            if (strlen(token) > 0) {
                int encontrado = 0;
                for (int i = 0; i < n_palabras; i++) {
                    if (strcmp(palabras[i], token) == 0) {
                        conteos[i]++;
                        encontrado = 1;
                        break;
                    }
                }

                if (!encontrado && n_palabras < MAX) {
                    palabras[n_palabras] = strdup(token); // Guardar copia de la palabra
                    conteos[n_palabras] = 1;
                    n_palabras++;
                }
            }

            token = strtok(NULL, " \t\n");
        }
    }

    fclose(file);

    // Determinar la palabra más repetida
    *max_reps = 0;
    strcpy(palabra_mas_repetida, "");
    for (int i = 0; i < n_palabras; i++) {
        if (conteos[i] > *max_reps) {
            *max_reps = conteos[i];
            strncpy(palabra_mas_repetida, palabras[i], MAX - 1);
            palabra_mas_repetida[MAX - 1] = '\0';
        }
        free(palabras[i]); // Liberar memoria
    }
}

// Función para recibir archivo y analizarlo
void recibir_y_analizar(int connfd, bool *finalizar) {
    FILE *file = fopen("archivo_recibido.txt", "w");
    if (file == NULL) {
        perror("Error al abrir archivo para escritura");
        return;
    }

    char buffer[MAX];
    int bytes_read;
    int eof_received = 0;

    printf("Recibiendo archivo...\n");

    // Leer datos en bloques hasta recibir "EOF"
    while ((bytes_read = read(connfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';

        // Verificar si se recibió "EOF"
        if (strstr(buffer, "EOF") != NULL) {
            eof_received = 1;
            char *eof_position = strstr(buffer, "EOF");
            *eof_position = '\0'; // Terminar antes del "EOF"
            fwrite(buffer, 1, strlen(buffer), file);
            break;
        }

        // Escribir los datos en el archivo
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);

    if (eof_received) {
        printf("Archivo recibido completamente y guardado como 'archivo_recibido.txt'.\n");
    } else {
        printf("El cliente terminó la conexión.\n");
        close(connfd);
        *finalizar = false;
        return;
    }

    // Analizar el archivo
    char palabra_mas_repetida[MAX];
    int max_reps;
    analizar_archivo("archivo_recibido.txt", palabra_mas_repetida, &max_reps);

    // Preparar respuesta
    char respuesta[MAX];
    snprintf(respuesta, sizeof(respuesta), "P:%s#C:%d",
             palabra_mas_repetida, max_reps);

    // Enviar respuesta al cliente
    write(connfd, respuesta, strlen(respuesta));
    printf("Resultados enviados al cliente: %s\n", respuesta);
}


int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket creado exitosamente.\n");

    // Server configuration
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind the socket
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
        perror("Error al enlazar el socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Socket enlazado al puerto %d.\n", PORT);

    // Listen for connections
    if ((listen(sockfd, 5)) != 0) {
        perror("Error al configurar el servidor para escuchar");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Servidor escuchando...\n");

    len = sizeof(cli);
    // Accept a client connection
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0) {
        perror("Error al aceptar conexión");
    }
    printf("Conexión aceptada.\n");
    bool finalizar = true;
    while (finalizar) {  // Infinite loop to handle multiple clients
        // Receive and analyze the file from the client
        recibir_y_analizar(connfd, &finalizar);
    }

    // Close the server socket (this is rarely reached due to the infinite loop)
    close(sockfd);
    printf("Servidor cerrado.\n");
    return 0;
}
