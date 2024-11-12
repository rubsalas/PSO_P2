// lcd_buzzer.c
#include "lcd_buzzer.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/lcd"  // Ruta al archivo de dispositivo

static int device_fd = -1;  // Descriptor de archivo para el dispositivo

int lcd_buzzer_init(void) {
    device_fd = open(DEVICE_PATH, O_WRONLY);
    if (device_fd < 0) {
        perror("Error al abrir el dispositivo");
        return -1;
    }
    return 0;
}

void lcd_buzzer_close(void) {
    if (device_fd >= 0) {
        close(device_fd);
        device_fd = -1;
    }
}

void play_melody(void) {
    if (device_fd < 0) {
        fprintf(stderr, "Dispositivo no abierto\n");
        return;
    }
    ioctl(device_fd, 0, NULL);  // Código 0 indica `play_melody`
}

void beep_buzzer(int times) {
    if (device_fd < 0) {
        fprintf(stderr, "Dispositivo no abierto\n");
        return;
    }
    ioctl(device_fd, 1, &times); // Código 1 indica `beep_buzzer`
}

int lcd_write(const char *message) {
    if (device_fd < 0) {
        fprintf(stderr, "Dispositivo no abierto\n");
        return -1;
    }
    ssize_t bytes_written = write(device_fd, message, strlen(message));
    if (bytes_written == -1) {
        perror("Error al escribir en el dispositivo");
        return -1;
    }
    printf("Texto enviado a la LCD: %s\n", message);
    return bytes_written;
}
