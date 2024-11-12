// main.c
#include "lcd_buzzer.h"
#include <stdio.h>
#include <string.h>

int main() {
    if (lcd_buzzer_init() < 0) {
        return -1;
    }
    const char *palabra = "Quijote";
    const char *cantidad = "1229";
    const char *cambio = "#";
    const char *final = " ";
    char linea1[16] = "P: ";
    char linea2[16] = "C: ";
    char lcd[32] = "Cantidad: ";
    // Inicializar el buffer vacío
    lcd[0] = '\0';
    strcat(linea1, palabra);
    strcat(linea1, cambio);
    strcat(linea2, cantidad);
    strcat(linea2, final);
    // Concatenar las cadenas
    strcat(lcd, linea1);  // Concatenar el primer mensaje
    strcat(lcd, linea2);  // Concatenar el segundo mensaje (espacio)

    lcd_write(lcd);

    play_melody();
    beep_buzzer(3);

    lcd_buzzer_close();
    return 0;
}
