# PSO_P2
Proyecto 2 del curso CE-4303 Principios de Sistemas Operativos del programa de Licenciatura en Ingeniería en Computadores del Instituto Tecnológico de Costa Rica.

## Proyecto Específico por Definir


### Driver

Para la utilización de este driver es esencial contar al menos con el lcd_driver.c

1. Compilación del Driver:

En el terminal, navega al directorio donde se encuentra tu código fuente del driver (lcd_driver.c) y el Makefile. Luego ejecuta el siguiente comando:
```bash
make
```

O podemos realizar la compilación sin la necesidad de un archivo make, ejecutando los comandos:
```bash
make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
make -C /lib/modules/$(uname -r)/build M=$(pwd) clean
```

Estos comandos compilarán el driver y generarán un archivo .ko (módulo del kernel), en este caso, lcd_driver.ko.

2. Instalación del driver (cargar el módulo):

Una vez compilado el módulo, puedes cargarlo al kernel de Linux usando insmod:
```bash
sudo insmod lcd_driver.ko
```

Para verificar que el módulo se haya cargado correctamente, puedes usar el siguiente comando:
```bash
lsmod | grep lcd_driver
```

3. Verificar que el Driver Está Funcionando

Puedes comprobar que tu dispositivo ha sido creado correctamente y que el módulo está activo verificando la creación de /dev/lcd (ruta definida para el dispositivo):
```bash
ls /dev/lcd
```

4. Eliminación del Driver

Si necesitas quitar el driver, puedes usar el siguiente comando:
```bash
sudo rmmod lcd_driver
```
Este comando elimina el módulo del kernel.
