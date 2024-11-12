#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/* Metainformación */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kendall Marín");
MODULE_DESCRIPTION("A driver to control an LCD 2*16 text display and a buzzer.");

/* Variables para dispositivo y clase de dispositivo */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "lcd"
#define DRIVER_CLASS "MyModuleClass"

/* LCD char buffer */
static char lcd_buffer[33];

/* Pinout para pantalla LCD */
unsigned int gpios[] = {
	3, /* Enable Pin */
	2, /* Register Select Pin */
	5, /* Data Pin 0*/
	11, /* Data Pin 1*/
	9, /* Data Pin 2*/
	10, /* Data Pin 3*/
	22, /* Data Pin 4*/
	27, /* Data Pin 5*/
	17, /* Data Pin 6*/
	4, /* Data Pin 7*/
};
/* Definir el pin del buzzer */
unsigned int buzzer_pin = 18;  

/* Notas para la melodía de Fur Elise */
#define NOTE_E5 659
#define NOTE_DS5 622
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C4 262
#define NOTE_E4 330
#define NOTE_GS4 415
#define NOTE_C5 523
#define NOTE_D5 587
#define REST 0

/* Tempo de la melodía */
int tempo = 80;
int wholenote;

const int melody[] = {
    NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16,
    NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, NOTE_B4, -8, NOTE_E4, 16, NOTE_GS4, 16, NOTE_B4, 16,
    NOTE_C5, 8,  REST, 16, NOTE_E4, 16, NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16, NOTE_DS5, 16, NOTE_E5, 16,
    NOTE_B4, 16, NOTE_D5, 16, NOTE_C5, 16, NOTE_A4, -8, NOTE_C4, 16, NOTE_E4, 16, NOTE_A4, 16, NOTE_B4, -8,
    NOTE_E4, 16, NOTE_C5, 16, NOTE_B4, 16, NOTE_A4, 4, REST, 8
};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;

/* Función para reproducir una nota en el buzzer */
void play_note(int frequency, int duration_ms) {
    if (frequency == REST) {
        msleep(duration_ms);  // Pausa
    } else {
        int delay = 1000000 / (2 * frequency);  // Período de la nota en microsegundos
        int cycles = (frequency * duration_ms) / 1000;  // Número de ciclos para duración
	int i;
        for (i = 0; i < cycles; i++) {
            gpio_set_value(buzzer_pin, 1);
            udelay(delay);
            gpio_set_value(buzzer_pin, 0);
            udelay(delay);
        }
    }
    msleep(duration_ms / 10);  // Pequeña pausa entre notas
}

/**
 * @brief Reproduce la melodía de Fur Elise en el buzzer
 */
void play_melody(void) {
	int divider, noteDuration;

	int i;
        for (i = 0; i < notes * 2; i += 2) {
        int frequency = melody[i];
        divider = melody[i + 1];

        if (divider > 0) {
            noteDuration = wholenote / divider;
        } else {
            noteDuration = ((wholenote / abs(divider)) * 3)/2;
        }

        play_note(frequency, noteDuration);
    }
}
#define REGISTER_SELECT gpios[1]

/* Función para hacer sonar el buzzer 3 veces */
void beep_buzzer(int times) {
    int i;
    for (i = 0; i < times; i++) {
        gpio_set_value(buzzer_pin, 1);  // Activa el buzzer
        msleep(500);                    // Espera 500 ms
        gpio_set_value(buzzer_pin, 0);  // Desactiva el buzzer
        msleep(500);                    // Espera 500 ms
    }
}


/**
 * @brief genera un pulso en el pin de enable
 */
void lcd_enable(void) {
	gpio_set_value(gpios[0], 1);
	msleep(5);
	gpio_set_value(gpios[0], 0);
}

/**
 * @brief configurar el bus de datos de 8 bits
 * @param data: Datos para configurar
 */
void lcd_send_byte(char data) {
	int i;
	for(i=0; i<8; i++)
		gpio_set_value(gpios[i+2], ((data) & (1<<i)) >> i);
	lcd_enable();
	msleep(5);
}

/**
 * @brief enviar un comando a la pantalla LCD
 *
 * @param data: comando para enviar
 */
void lcd_command(uint8_t data) {
 	gpio_set_value(REGISTER_SELECT, 0);	/* RS a Instrucción */
	lcd_send_byte(data);
}

/**
 * @brief enviar datos a la pantalla LCD
 *
 * @param data: comando para enviar
 */
void lcd_data(uint8_t data) {
 	gpio_set_value(REGISTER_SELECT, 1);	/* RS a datos */
	lcd_send_byte(data);
}


/**
 * @brief Escribir datos en el buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta, i;

	/* Obtener cantidad de datos para copiar */
	to_copy = min(count, sizeof(lcd_buffer));

	/* Copiar datos al usuario */
	not_copied = copy_from_user(lcd_buffer, user_buffer, to_copy);

	/* Calcular datos */
	delta = to_copy - not_copied;

	/* Establecer los nuevos datos en la pantalla. */
	lcd_command(0x01);
	
	for(i=0; i<to_copy-1; i++){
		if (lcd_buffer[i] == '#') {
			lcd_command(0xC0);  // Comando para segunda línea
			i++;
			}
		lcd_data(lcd_buffer[i]);
		}

	return delta;
}


/**
 * @brief Esta función se llama cuando se abre el archivo del dispositivo.
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief Esta función se llama cuando se cierra el archivo del dispositivo.
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.write = driver_write
};

/**
 * @brief Esta función se llama cuando el módulo se carga en el kernel
 */
static int __init ModuleInit(void) {
    int i;
    char *names[] = {"ENABLE_PIN", "REGISTER_SELECT", "DATA_PIN0", "DATA_PIN1", "DATA_PIN2", "DATA_PIN3", "DATA_PIN4", "DATA_PIN5", "DATA_PIN6", "DATA_PIN7"};
    printk("Hello, Kernel!\n");
    wholenote = (60000 * 4) / tempo;
    /* Asignar un número de dispositivo */
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }
    printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

    /* Crear clase de dispositivo */
    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class can not be created!\n");
        goto ClassError;
    }

    /* Crear archivo de dispositivo */
    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("Can not create device file!\n");
        goto FileError;
    }

    /* Inicializar archivo de dispositivo */
    cdev_init(&my_device, &fops);

    /* Registrar dispositivo en el kernel */
    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk("lcd-driver - Registering of device to kernel failed!\n");
        goto AddError;
    }

    /* Inicializar GPIOs */
    printk("lcd-driver - GPIO Init\n");
    for (i = 0; i < 10; i++) {
        if (gpio_request(gpios[i], names[i])) {
            printk("lcd-driver - Error Init GPIO %d\n", gpios[i]);
            goto GpioInitError;
        }
    }

    /* Configurar el GPIO del buzzer */
    if (gpio_request(buzzer_pin, "BUZZER_PIN")) {
        printk("lcd-driver - Error Init GPIO for buzzer\n");
        goto GpioBuzzerInitError;
    }
    if (gpio_direction_output(buzzer_pin, 0)) {
        printk("lcd-driver - Error setting GPIO for buzzer to output\n");
        goto GpioBuzzerDirectionError;
    }

    printk("lcd-driver - Set GPIOs to output\n");
    for (i = 0; i < 10; i++) {
        if (gpio_direction_output(gpios[i], 0)) {
            printk("lcd-driver - Error setting GPIO %d to output\n", i);
            goto GpioDirectionError;
        }
    }

    /* Inicializar la pantalla LCD */
    lcd_command(0x38);   /* Configurar la pantalla para 8 bits, 2 lÃ­neas, fuente 5x8 */
    lcd_command(0x0C);   /* Pantalla encendida, cursor apagado */
    lcd_command(0x06);   /* Modo de entrada: Auto-incremento, sin desplazamiento */
    lcd_command(0x01);   /* Limpiar pantalla */
    msleep(5);           /* Retraso para que el comando de limpieza termine */

    char text[] = "Palabra:Hola\nCantidad:39!";
    for (i = 0; i < sizeof(text) - 1; i++) {
        if (text[i] == '\n') {
            lcd_command(0xC0);  // Comando para la segunda línea
            i++;
        }
        lcd_data(text[i]);
    }

    /* Hacer sonar el buzzer 3 veces */
    beep_buzzer(3);

    return 0;

GpioBuzzerDirectionError:
    gpio_free(buzzer_pin);
GpioBuzzerInitError:
    for (i = 0; i < 10; i++) {
        gpio_free(gpios[i]);
    }
GpioDirectionError:
    for (i = 0; i < 10; i++) {
        gpio_free(gpios[i]);
    }
GpioInitError:
    for (i = 0; i < 10; i++) {
        gpio_free(gpios[i]);
    }
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev_region(my_device_nr, 1);
    return -1;
}

/**
 * @brief Esta función se llama cuando el módulo se elimina del kernel
 */
static void __exit ModuleExit(void) {
    int i;
    lcd_command(0x1);    /* Limpiar la pantalla y liberar */
    for (i = 0; i < 10; i++) {
        gpio_set_value(gpios[i], 0);
        gpio_free(gpios[i]);
    }
    gpio_set_value(buzzer_pin, 0);  /* Asegurarse de que el buzzer está apagado */
    gpio_free(buzzer_pin);          /* Liberar el GPIO del buzzer */
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);