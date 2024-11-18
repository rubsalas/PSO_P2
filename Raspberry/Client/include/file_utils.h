#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdbool.h>

/*
 * Función: list_files
 * ----------------------
 * Lista todos los archivos con extensión ".txt" en el directorio actual.
 * Muestra los nombres de los archivos encontrados en la salida estándar.
 */
void list_files(void);

/*
 * Función: send_file
 * ----------------------
 * Envía un archivo al servidor a través de un socket.
 * 
 * Parámetros:
 *    sockfd - Descriptor del socket a través del cual se enviará el archivo
 *    filename - Nombre del archivo que se desea enviar
 */
void send_file(int sockfd, const char *filename);

/*
 * Función: file_exists
 * ----------------------
 * Verifica si un archivo existe en el directorio `TEXTS_DIR`.
 * 
 * Parámetros:
 *    filename - Nombre del archivo a verificar
 * 
 * Retorna:
 *    true si el archivo existe, false de lo contrario.
 */
bool file_exists(const char *filename);

/*
 * Función: generate_full_path
 * ----------------------
 * Genera la ruta completa de un archivo basado en `TEXTS_DIR` y el nombre
 * del archivo proporcionado.
 * 
 * Parámetros:
 *    filename - Nombre del archivo
 *    full_path - Buffer donde se almacenará la ruta completa generada
 * 
 * Retorna:
 *    Un puntero a la ruta completa (igual a `full_path`).
 */
char* generate_full_path(const char *filename, char *full_path);

#endif // FILE_UTILS_H
