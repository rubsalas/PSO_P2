#include <stdio.h>
#include <stdbool.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> // Funciones: read(), write(), close()
#include <ctype.h>  // Función: isalpha()

#include "mpi_utils.h"
#include "word_counter.h"
#include "server.h"

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

void recibir(int connfd, bool *finalizar) {
    // Abrir un archivo para escribir los datos recibidos
    FILE *file = fopen("runtime/archivo_recibido.txt", "w");
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

}

// Función para recibir un archivo desde el cliente y analizarlo
void recibir_y_analizar(int rank, int size, int connfd, bool *finalizar) {


    // // Analizar el archivo recibido para encontrar la palabra más repetida
    // char palabra_mas_repetida[MAX]; // Almacena la palabra más repetida
    // int max_reps;                   // Almacena el número de repeticiones de la palabra más repetida
    // analizar_archivo("archivo_recibido.txt", palabra_mas_repetida, &max_reps);


    WordCount* globalWordCounts = NULL;
    int globalCountSize = 0;

    if (rank == 0) {
        recibir(connfd, finalizar);
        distribute_text("runtime/archivo_recibido.txt", size);
    } else {
        receive_and_count_fragment(rank);
    }

    if (rank == 0) {
        consolidate_counts(size, &globalWordCounts, &globalCountSize);

        // Encontrar la palabra más repetida
        qsort(globalWordCounts, globalCountSize, sizeof(WordCount), compareWordCount);
        printf("Palabra más repetida: %s, Frecuencia: %d\n", globalWordCounts[0].word, globalWordCounts[0].frequency);


        // Preparar la respuesta a enviar al cliente en el formato "P:palabra#C:conteo"
        char respuesta[MAX];
        snprintf(respuesta, sizeof(respuesta), "P:%s#C:%d",
                globalWordCounts[0].word, globalWordCounts[0].frequency);
                

        // Encriptar y desencriptar el resultado

        unsigned char key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        unsigned char iv[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

        unsigned char encryptedText[128];
        int encryptedTextLength = encrypt((unsigned char*)respuesta, strlen(respuesta), key, iv, encryptedText);

        if (encryptedTextLength == -1) {
            fprintf(stderr, "Error al encriptar el texto\n");
            // Manejo de errores adicional
        }

        // Crear una cadena para almacenar el texto encriptado en formato hexadecimal
        char encrypted_response[MAX * 2]; // Cada byte en hexadecimal ocupa 2 caracteres
        int pos = 0;

        // Convertir el texto encriptado a una cadena hexadecimal y almacenarla en 'encrypted_response'
        for (int i = 0; i < encryptedTextLength; ++i) {
            pos += snprintf(encrypted_response + pos, sizeof(encrypted_response) - pos, "%02x", encryptedText[i]);
        }

        // Ahora 'encrypted_response' contiene el texto encriptado en formato hexadecimal
        printf("Texto encriptado: %s\n", encrypted_response);

        // Enviar el texto encriptado al cliente
        write(connfd, encryptedText, encryptedTextLength);
        printf("Resultados encriptados enviados al cliente.\n");

    }
}

int main(int argc, char** argv) {

    int rank, size;
    mpi_initialize(&argc, &argv, &rank, &size);

    int server_fd;
    int client_fd;

    if (rank == 0) {
        server_fd = initialize_server();
        if (server_fd < 0) {
            return 1;
        }

        client_fd = accept_client(server_fd);
        if (client_fd < 0) {
            perror("Error al aceptar conexión");
        }
    }   

    bool finalizar = true; // Bandera para controlar el bucle principal
    while (finalizar) {    // Bucle para manejar la comunicación con el cliente
        // Recibir y analizar el archivo enviado por el cliente
        recibir_y_analizar(rank, size, client_fd, &finalizar);
    }

    if (rank == 0) {
        // Cerrar el socket del servidor (normalmente no se alcanza debido al bucle infinito)
        close(server_fd);
        printf("Servidor cerrado.\n");
    }
    return 0;
}
