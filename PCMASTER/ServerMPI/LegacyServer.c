#include <stdio.h>
#include <stdbool.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> // Funciones: read(), write(), close()
#include <ctype.h>  // Función: isalpha()

// Definición de constantes
#define MAX 1024           // Tamaño máximo para buffers y arreglos
#define PORT 8080          // Puerto en el que el servidor escuchará conexiones
#define SA struct sockaddr // Alias para la estructura sockaddr

// Función para analizar un archivo y determinar la palabra más repetida
void analizar_archivo(const char *filename, char *palabra_mas_repetida, int *max_reps) {
    // Abrir el archivo en modo lectura
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // Si ocurre un error al abrir el archivo, se muestra un mensaje y se retornan valores de error
        perror("Error al abrir el archivo para análisis");
        strcpy(palabra_mas_repetida, "Error");
        *max_reps = -1;
        return;
    }

    // Declaración de buffers y arreglos para almacenar palabras y sus conteos
    char buffer[MAX];        // Buffer para leer líneas del archivo
    char copia[MAX];         // Copia de la línea leída para tokenización
    char *palabras[MAX];     // Arreglo de punteros a cadenas para almacenar palabras únicas
    int conteos[MAX] = {0};  // Arreglo de conteos para cada palabra única
    int n_palabras = 0;      // Contador de palabras únicas encontradas

    // Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Hacer una copia de la línea leída para tokenizar
        strncpy(copia, buffer, sizeof(copia) - 1);
        copia[sizeof(copia) - 1] = '\0'; // Asegurar la terminación de la cadena

        // Tokenizar la línea utilizando espacios, tabulaciones y saltos de línea como delimitadores
        char *token = strtok(copia, " \t\n");
        while (token != NULL) {
            // Limpiar la palabra eliminando caracteres no alfabéticos al inicio y al final
            int start = 0;
            int end = strlen(token) - 1;

            // Mover el índice 'start' hasta el primer carácter alfabético
            while (start <= end && !isalpha(token[start])) start++;
            // Mover el índice 'end' hasta el último carácter alfabético
            while (end >= start && !isalpha(token[end])) end--;

            // Ajustar la cadena para que sólo contenga caracteres alfabéticos
            token[end + 1] = '\0';
            token += start;

            // Procesar la palabra si no está vacía después de la limpieza
            if (strlen(token) > 0) {
                int encontrado = 0; // Bandera para indicar si la palabra ya se encontró

                // Buscar si la palabra ya ha sido encontrada anteriormente
                for (int i = 0; i < n_palabras; i++) {
                    if (strcmp(palabras[i], token) == 0) {
                        // Si la palabra ya existe, incrementar su conteo
                        conteos[i]++;
                        encontrado = 1;
                        break;
                    }
                }

                // Si la palabra no se ha encontrado antes, agregarla al arreglo de palabras
                if (!encontrado && n_palabras < MAX) {
                    palabras[n_palabras] = strdup(token); // Almacenar una copia de la palabra
                    conteos[n_palabras] = 1;              // Inicializar el conteo a 1
                    n_palabras++;                         // Incrementar el contador de palabras únicas
                }
            }

            // Obtener el siguiente token (palabra)
            token = strtok(NULL, " \t\n");
        }
    }

    // Cerrar el archivo después de terminar la lectura
    fclose(file);

    // Determinar la palabra más repetida entre todas las palabras almacenadas
    *max_reps = 0;                     // Inicializar el máximo de repeticiones
    strcpy(palabra_mas_repetida, "");  // Inicializar la palabra más repetida

    for (int i = 0; i < n_palabras; i++) {
        if (conteos[i] > *max_reps) {
            // Si se encuentra una palabra con un conteo mayor, actualizar los valores
            *max_reps = conteos[i];
            strncpy(palabra_mas_repetida, palabras[i], MAX - 1);
            palabra_mas_repetida[MAX - 1] = '\0'; // Asegurar la terminación de la cadena
        }
        // Liberar la memoria asignada para la palabra
        free(palabras[i]);
    }
}

// Función para recibir un archivo desde el cliente y analizarlo
void recibir_y_analizar(int connfd, bool *finalizar) {
    // Abrir un archivo para escribir los datos recibidos
    FILE *file = fopen("archivo_recibido.txt", "w");
    if (file == NULL) {
        // Si ocurre un error al abrir el archivo, mostrar mensaje de error
        perror("Error al abrir archivo para escritura");
        return;
    }

    char buffer[MAX];       // Buffer para almacenar datos recibidos
    int bytes_read;         // Número de bytes leídos en cada iteración
    int eof_received = 0;   // Bandera para indicar si se ha recibido "EOF"

    printf("Recibiendo archivo...\n");

    // Leer datos desde el socket en bloques hasta recibir "EOF"
    while ((bytes_read = read(connfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Asegurar la terminación de la cadena

        // Verificar si se recibió "EOF" indicando el final de la transmisión
        if (strstr(buffer, "EOF") != NULL) {
            eof_received = 1; // Marcar que se recibió "EOF"
            char *eof_position = strstr(buffer, "EOF"); // Obtener posición de "EOF"
            *eof_position = '\0'; // Terminar la cadena antes de "EOF"
            // Escribir los datos antes de "EOF" en el archivo
            fwrite(buffer, 1, strlen(buffer), file);
            break; // Salir del bucle de lectura
        }

        // Escribir los datos recibidos en el archivo
        fwrite(buffer, 1, bytes_read, file);
    }

    // Cerrar el archivo después de terminar la escritura
    fclose(file);

    if (eof_received) {
        printf("Archivo recibido completamente y guardado como 'archivo_recibido.txt'.\n");
    } else {
        printf("El cliente terminó la conexión.\n");
        close(connfd);      // Cerrar la conexión con el cliente
        *finalizar = false; // Indicar que se debe finalizar el bucle principal
        return;
    }

    // Analizar el archivo recibido para encontrar la palabra más repetida
    char palabra_mas_repetida[MAX]; // Almacena la palabra más repetida
    int max_reps;                   // Almacena el número de repeticiones de la palabra más repetida
    analizar_archivo("archivo_recibido.txt", palabra_mas_repetida, &max_reps);

    // Preparar la respuesta a enviar al cliente en el formato "P:palabra#C:conteo"
    char respuesta[MAX];
    snprintf(respuesta, sizeof(respuesta), "P:%s#C:%d",
             palabra_mas_repetida, max_reps);

    // Enviar la respuesta al cliente a través del socket
    write(connfd, respuesta, strlen(respuesta));
    printf("Resultados enviados al cliente: %s\n", respuesta);
}

int main() {
    int sockfd;   // Descriptor para el socket del servidor
    int connfd;   // Descriptor para la conexión con el cliente
    int len;      // Longitud de la estructura sockaddr_in
    struct sockaddr_in servaddr; // Estructura para la dirección del servidor
    struct sockaddr_in cli;      // Estructura para la dirección del cliente

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

    len = sizeof(cli);

    // Aceptar una conexión de un cliente
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0) {
        // Si ocurre un error al aceptar la conexión, mostrar mensaje
        perror("Error al aceptar conexión");
    }
    printf("Conexión aceptada.\n");

    bool finalizar = true; // Bandera para controlar el bucle principal
    while (finalizar) {    // Bucle para manejar la comunicación con el cliente
        // Recibir y analizar el archivo enviado por el cliente
        recibir_y_analizar(connfd, &finalizar);
    }

    // Cerrar el socket del servidor (normalmente no se alcanza debido al bucle infinito)
    close(sockfd);
    printf("Servidor cerrado.\n");
    return 0;
}
