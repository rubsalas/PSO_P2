#ifndef LCD_BUZZER_H
#define LCD_BUZZER_H

// Inicializa el controlador del LCD y el buzzer
int lcd_buzzer_init(void);

// Cierra el dispositivo
void lcd_buzzer_close(void);

// Reproduce la melodí­a Fur Elise
void play_melody(void);

// Hace sonar el buzzer un número especí­fico de veces
void beep_buzzer(int times);

// Escribe un mensaje en la pantalla LCD
int lcd_write(const char *message);

#endif // LCD_BUZZER_H
