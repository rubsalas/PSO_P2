#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/lcd"  // Ruta al archivo de dispositivo LCD

int main() {
    int fd;
    char *message = "Palabra: Quijote#Cantidad: 1229 ";  // El texto que deseas mostrar en la LCD

    // Abre el archivo de dispositivo para escribir
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd == -1) {
        perror("No se pudo abrir el archivo de dispositivo");
        return 1;
    }

    // Escribe el mensaje en la LCD
    ssize_t bytes_written = write(fd, message, strlen(message));
    if (bytes_written == -1) {
        perror("Error al escribir en el dispositivo");
        close(fd);
        return 1;
    }

    printf("Texto enviado a la LCD: %s\n", message);

    // Cierra el archivo de dispositivo
    close(fd);

    return 0;
}
