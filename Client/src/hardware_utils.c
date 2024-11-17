#include "hardware_utils.h"
//#include <lcd_buzzer.h> // DRIVER NOT INSTALLED YET
#include <unistd.h> // Para usleep (pausas en microsegundos)

/*
 * Función: hardware_init
 * ----------------------
 * Inicializa los componentes de hardware, como el LCD y el buzzer.
 * Esta función configura los pines necesarios y verifica que los dispositivos estén
 * listos para ser utilizados.
 * 
 * Retorna:
 *    0 si la inicialización fue exitosa,
 *   -1 si ocurrió un error durante la inicialización.
 */
int hardware_init(void) {
    // if (lcd_buzzer_init() < 0) {     // DRIVER NOT INSTALLED YET
    //     fprintf(stderr, "Error al inicializar el hardware.\n");
    //     return -1;
    // }
    printf("Hardware inicializado correctamente.\n");
    return 0;
}

/*
 * Función: hardware_display_message
 * ----------------------
 * Muestra un mensaje de texto en el LCD. La función asegura que el texto se ajuste
 * al tamaño del LCD o se recorte si es necesario.
 * 
 * Parámetros:
 *    message - Puntero a la cadena de texto que se desea mostrar en el LCD.
 */
void hardware_display_message(const char *message) {
    //lcd_write(message); // Escribe el mensaje en el LCD   // DRIVER NOT INSTALLED YET
    printf("Mensaje en LCD: %s\n", message);
}

/*
 * Función: hardware_play_melody
 * ----------------------
 * Reproduce una melodía predefinida utilizando el buzzer. 
 * La melodía puede ser configurada para diferentes propósitos, como alertas.
 */
void hardware_play_melody(void) {
    //play_melody();  // DRIVER NOT INSTALLED YET
    printf("Melodía reproducida en el buzzer.\n");
}

/*
 * Función: hardware_beep
 * ----------------------
 * Emite un número específico de pitidos cortos utilizando el buzzer. Cada pitido
 * tiene una duración fija, y se puede utilizar para señales o alertas.
 * 
 * Parámetros:
 *    count - Número de pitidos a emitir.
 */
void hardware_beep(int count) {
    for (int i = 0; i < count; i++) {
        //beep_buzzer(1); // Un pitido    // DRIVER NOT INSTALLED YET
        usleep(300000); // Pausa de 300 ms entre pitidos
    }
    printf("Buzzer emitió %d pitidos.\n", count);
}

/*
 * Función: hardware_close
 * ----------------------
 * Cierra y libera los recursos utilizados por el hardware, como el LCD y el buzzer.
 * Esta función asegura que los dispositivos queden en un estado seguro antes de
 * finalizar el programa.
 */
void hardware_close(void) {
    //lcd_buzzer_close();   // DRIVER NOT INSTALLED YET
    printf("Hardware cerrado correctamente.\n");
}
