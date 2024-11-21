#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Bibliotecas locales
#include "lcd_buzzer/lcd_buzzer.h"
#include "socket_utils.h"
#include "file_utils.h"
#include "crypto_utils.h"

#define MAX 1024
#define SERVER_IP "172.18.64.131" // Dirección IP del servidor

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
    if (lcd_buzzer_init() < 0) {
        return -1;
    }

    // Conectar al servidor
    int sockfd = socket_connect(SERVER_IP, 8080);
    if (sockfd < 0) {
        lcd_buzzer_close(); // Liberar recursos si falla la conexión
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
                
                //printf("Respuesta del servidor (Texto encriptado): %s\n", response);

                // Desencriptar la respuesta
                unsigned char key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
                unsigned char iv[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

                unsigned char decryptedText[128];
                int decryptedTextLength = decrypt((unsigned char*)response, strlen(response), key, iv, decryptedText);
                
                if (decryptedTextLength == -1) {
                    fprintf(stderr, "Error al desencriptar el texto\n");
                    // Manejo de errores
                }

                decryptedText[decryptedTextLength] = '\0';

                // Mostrar el texto desencriptado
                printf("Texto desencriptado: %s\n", decryptedText);

                // Mostrar respuesta del servidor en el hardware y consola
                char lcd[32];
                const char *final = " ";
                lcd[0]= '\0';
                strcat(lcd, decryptedText);  // Concatenar mensaje
                strcat(lcd, final);
                lcd_write(lcd);
	            play_melody();
                beep_buzzer(3);

            } else {
                printf("No se recibió respuesta del servidor.\n");
            }
        } else {
            printf("El archivo '%s' no existe. Intente de nuevo.\n", input);
        }
    }

    // Cerrar conexión y liberar recursos del hardware
    close(sockfd);
    lcd_buzzer_close();
    printf("Conexión cerrada.\n");
    return 0;
}
