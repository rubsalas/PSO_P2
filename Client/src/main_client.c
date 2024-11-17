#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket_utils.h"
#include "file_utils.h"
#include "hardware_utils.h"

#define MAX 1024
#define SERVER_IP "192.168.50.100" // Dirección IP del servidor

/*
 * Función: main
 * ----------------------
 * Punto de entrada principal del cliente. Maneja la inicialización del hardware,
 * la conexión al servidor y la interacción del usuario para enviar archivos.
 * 
 * Retorna:
 *    0 si el programa finaliza correctamente,
 *   -1 si ocurre un error durante la ejecución.
 */
int main() {
    // Inicializar hardware (LCD y buzzer)
    if (hardware_init() < 0) {
        return -1;
    }

    // Conectar al servidor
    int sockfd = socket_connect(SERVER_IP, 8080);
    if (sockfd < 0) {
        hardware_close(); // Liberar recursos si falla la conexión
        return -1;
    }

    printf("Conectado al servidor (%s).\n", SERVER_IP);

    // Bucle principal para interacción con el usuario
    while (1) {
        list_files(); // Mostrar los archivos disponibles en el directorio
        printf("\nEscriba el nombre del archivo a enviar o 'salir' para cerrar la conexión: ");

        char input[MAX];

        // Leer entrada del usuario
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Eliminar salto de línea al final

        // Salir si el usuario escribe "salir"
        if (strcmp(input, "salir") == 0) {
            printf("Cerrando cliente.\n");
            break;
        }

        // Verificar si el archivo existe
        if (file_exists(input)) {
            send_file(sockfd, input); // Enviar el archivo al servidor
            char response[MAX];
            if (socket_receive(sockfd, response, sizeof(response)) > 0) {
                // Mostrar respuesta del servidor en el hardware y consola
                hardware_display_message(response);
                hardware_play_melody();
                hardware_beep(3);
                printf("Respuesta del servidor:\n%s\n", response);
            } else {
                printf("No se recibió respuesta del servidor.\n");
            }
        } else {
            printf("El archivo '%s' no existe. Intente de nuevo.\n", input);
        }
    }

    // Cerrar conexión y liberar recursos del hardware
    close(sockfd);
    hardware_close();
    printf("Conexión cerrada.\n");
    return 0;
}
