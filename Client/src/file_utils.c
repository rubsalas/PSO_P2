#include "file_utils.h"
#include <stdio.h>    // Para printf, perror
#include <dirent.h>   // Para manipular directorios
#include <string.h>   // Para manejo de cadenas
#include <unistd.h>   // Para access()

#define MAX 1024
#define TEXTS_DIR "../Texts/"

/*
 * Función: list_files
 * ----------------------
 * Lista todos los archivos con extensión ".txt" en el directorio especificado por `TEXTS_DIR`.
 * Imprime los nombres de los archivos encontrados en la salida estándar.
 */
void list_files(void) {
    DIR *dir;
    struct dirent *entry;

    printf("\n--- Archivos disponibles para enviar ---\n");
    if ((dir = opendir(TEXTS_DIR)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            // Verificar si el archivo tiene la extensión ".txt"
            if (strstr(entry->d_name, ".txt") != NULL) {
                printf(" - %s\n", entry->d_name);
            }
        }
        closedir(dir); // Cerrar el directorio después de procesarlo
    } else {
        perror("No se pudo abrir el directorio"); // Imprimir mensaje de error si no se puede abrir
    }
}

/*
 * Función: send_file
 * ----------------------
 * Envía un archivo al servidor a través de un socket. 
 * La función construye la ruta completa del archivo usando `TEXTS_DIR` y luego
 * envía el contenido del archivo en bloques.
 * 
 * Parámetros:
 *    sockfd - Descriptor del socket para enviar los datos
 *    filename - Nombre del archivo que se desea enviar
 */
void send_file(int sockfd, const char *filename) {
    char full_path[MAX];
    snprintf(full_path, MAX, "%s%s", TEXTS_DIR, filename); // Construir la ruta completa

    FILE *file = fopen(full_path, "r"); // Abrir el archivo en modo lectura
    if (file == NULL) {
        perror("Error al abrir el archivo"); // Imprimir mensaje si ocurre un error al abrir
        return;
    }

    char buffer[MAX];
    size_t bytes_read;

    // Leer y enviar el archivo en bloques
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (write(sockfd, buffer, bytes_read) != bytes_read) {
            perror("Error al enviar datos"); // Imprimir mensaje si ocurre un error al enviar
            fclose(file); // Cerrar el archivo antes de salir
            return;
        }
    }

    // Enviar un mensaje especial "EOF" para indicar el fin del archivo
    strcpy(buffer, "EOF");
    write(sockfd, buffer, strlen(buffer));

    printf("Archivo '%s' enviado correctamente.\n", filename);
    fclose(file); // Cerrar el archivo después de enviarlo
}

/*
 * Función: file_exists
 * ----------------------
 * Verifica si un archivo existe en el directorio `TEXTS_DIR`. Construye
 * la ruta completa del archivo y usa `access` para verificar su existencia.
 * 
 * Parámetros:
 *    filename - Nombre del archivo a verificar
 * 
 * Retorna:
 *    true si el archivo existe, false de lo contrario.
 */
bool file_exists(const char *filename) {
    char full_path[MAX];
    generate_full_path(filename, full_path); // Construir la ruta completa
    return (access(full_path, F_OK) == 0);  // Verificar si el archivo existe
}

/*
 * Función: generate_full_path
 * ----------------------
 * Genera la ruta completa de un archivo concatenando el directorio base `TEXTS_DIR`
 * y el nombre del archivo proporcionado.
 * 
 * Parámetros:
 *    filename - Nombre del archivo
 *    full_path - Buffer donde se almacenará la ruta completa generada
 * 
 * Retorna:
 *    Un puntero a la ruta completa almacenada en `full_path`.
 */
char* generate_full_path(const char *filename, char *full_path) {
    snprintf(full_path, MAX, "%s%s", TEXTS_DIR, filename); // Construir la ruta completa
    return full_path;
}
