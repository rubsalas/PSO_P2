# PSO_P2
Proyecto 2 del curso CE-4303 Principios de Sistemas Operativos del programa de Licenciatura en Ingeniería en Computadores del Instituto Tecnológico de Costa Rica.

## Proyecto Específico por Definir


### Biblioteca

1. Instalar la biblioteca:
Para utilizar la biblioteca debes aseugarte de incluir el archivo .h a tu programa.
Para utilizar la biblioteca en cualquier programa, es necesario copiar el archivo "liblcd_buzzer" a un directorio estándar como /usr/local/lib. Usa el siguiente comando:
```bash
sudo cp liblcd_buzzer.a /usr/local/lib/
```

Además, debes asegurarte que el compilador pueda encontrar los encabezados de la biblioteca, por lo que se debe copiar el archivo .h asociado a la biblioteca en /usr/local/include. Usando el siguiente comando:
```bash
sudo cp lcd_buzzer.h /usr/local/include/
```

2. Uso de la biblioteca:

Una vez instalado el driver y la biblioteca, es posible incluir la biblioteca a los programas de C.
```bash
#include "lcd_driver_lib.h"  // Asegúrate de que el archivo .h esté en el directorio correcto
```

3. Vincular la biblioteca al compilar:

Cuando compile el programa que utiliza la biblioteca, debes indicar al compilador que enlace la biblioteca estática .a generada utilizando los siguientes parametros:
```bash
gcc main.c -L/usr/local/lib -llcd_driver -o main_program
```
-L/usr/local/lib: Especifica la ruta donde está la biblioteca.
-llcd_driver: Le indica al compilador que enlace la biblioteca liblcd_driver.a.
-o main_program: Especifica el nombre del ejecutable final.

4. Funciones:

driver_write(message): Recibe como parametro un mensaje de máximo 32 caractéres, el cual mostrará en la pantalla LCD

play_melody(): Reproduce una melodía por el buzzer (Fur Elise).

beep_buzzer(3): Hace sonar el buzzer 3 veces.
