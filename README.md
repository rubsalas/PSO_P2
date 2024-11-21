# PSO_P2

Proyecto 2 del curso CE-4303 Principios de Sistemas Operativos del programa de Licenciatura en Ingeniería en Computadores del Instituto Tecnológico de Costa Rica.



## MPI Word Counter

**Índice**
I. Instalación de OpenMPI y Configuración de un Cluster
II. ServerMPI
III. Monitoreo de Tráfico de Red en Tiempo Real
IV. Imagen Raspberry Pi
V. Driver
VI. Biblioteca
VII. Cliente



### I. Instalación de OpenMPI y Configuración de un Cluster

1. Instalación de paquetes básicos en **TODOS** los nodos.

Descargar la versión más estable de [Open MPI](https://www.open-mpi.org/software/).

Crear un directorio en $HOME donde instalarlo.

```bash
mkdir  $HOME/openmpi
```

Mover el archivo comprimido que se descargó al directorio recién hecho.

```bash
mv  $HOME/Downloads/openmpi-X.X.X.tar.gz  $HOME/openmpi/
```

Ir al directorio creado por medio de la terminal.

```bash
cd  $HOME/openmpi/
```

Extraer el paquete.

```bash
tar  -xzvf  openmpi-X.X.X.tar.gz
```

Ir al directorio donde se descomprime el paquete.

```bash
cd  $HOME/openmpi/openmpi-X.X.X
```

Instalar build-essential.

```bash
sudo  apt-get  install  build-essential
```

Configurar donde instalar Open MPI.

```bash
sudo  ./configure  --prefix=/usr
```

Compilar el paquete de MPI en paralelo, utilizando todos los nucleos de la CPU para completar la compilación más rápido.

```bash
sudo  make  -j$(nproc)  all
```

Una vez compilado, instalar.

```bash
sudo  make  install
```

Instalar paquetes complementarios para OpenMPI.

```bash
sudo  apt-get  install  openmpi-bin  libopenmpi-dev
```

Agregar OpenMPI a las variables de entorno.

```bash
echo  'export PATH=/usr/bin:$PATH' >> ~/.bashrc
```
```bash
echo  'export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
```

Recargar el archivo ~/.bashrc para actualizar las variables de entorno.

```bash
source  ~/.bashrc
```

Verificar la instalación con la versión del compilador de c en OpenMPI.

```bash
mpicc  -v
```

**Si hay problemas con la instalación y configuración, volver a instalar OpenMPI desde cero.**

Eliminar archivos de compilación previos desde el directorio de OpenMPI.

```bash
make  clean
```

Ejecutar un distclean, que es una limpieza más profunda.

```bash
make  distclean
```

Proceder a desinstalarlo.

```bash
make  uninstall
```

Asegurarse donde se instaló OpenMPI para eliminar los folders que quedan.

```bash
which  mpirun
```

Eliminar todos los archivos de OpenMPI.

> Si está en /usr/bin/

```bash
sudo  rm  -rf  /usr/bin/mpi*
```
```bash
sudo  rm  -rf  /usr/include/mpi*
```
```bash
sudo  rm  -rf  /usr/lib/libmpi*
```
```bash
sudo  rm  -rf  /usr/share/openmpi
```
```bash
sudo  rm  -rf  /usr/etc/openmpi
```

2. Configuración de red estática en el **nodo esclavo**

Ir a la parte de la red cableada en la configuración de red (**nodo esclavo**)

En la parte de IPv4:

Asignar el método manual.

**Asignar una Dirección:**

- Dirección: 172.18.64.131
- Máscara de red: 255.255.255.0
- Puerta de enlace: 172.18.127.1

**Configurar el DNS:**

Desmarcar el automático.

- DNS: 172.20.10.15

Desconectarse y volverse a conectar para que los cambios ocurran.

3. Descarga de dependencias para la configuración del cluster

Instalar dependencias en el **nodo esclavo**.

```bash
sudo  apt-get  install  ssh
```
```bash
sudo  apt-get  install  nfs-common  portmap
```

Instalar dependencias en el **nodo maestro**.

```bash
sudo  apt-get  install  nfs-kernel-server  nfs-common  portmap
```

4. Configuración de red estática en el **nodo maestro**

Ir a la parte de la red cableada en la configuración de red.

En la parte de IPv4:

Asignar el método manual.

**Asignar una Dirección:**

- Dirección: 172.18.244.159
- Máscara de red: 255.255.255.0
- Puerta de enlace: 172.18.127.1

**Configurar el DNS:**

Desmarcar el automático.

- DNS: 172.20.10.15

Proceder a conectar el nodo maestro en la red cableada para que el LAN funcione.

5. Configuración del protocolo ssh

Para que el maestro sea capaz de ejecutar comandos en cada uno de los nodos esclavos se necesita que estos últimos puedan tener acceso al ssh.

Se crea una llave para el ssh en el **nodo maestro**.

```bash
ssh-keygen  -t  rsa
```

No asignar file.
Dejar empty el passphrase.

Se crea el key de forma aleatoria.

Revisar el nombre del public key que se acaba de crear, en este caso fue id_rsa.pub. Este está guardado en la carpeta .ssh/.

Se procede a copiar la llave que se acaba de crear al nodo esclavo, por medio del protocolo SCP.

```bash
scp  .ssh/id_rsa.pub  slaveUser@192.168.50.102:/home/slaveUser
```

Ingresar la clave del computador al que se está enviando, si es necesario.

Ahora en el **nodo esclavo**.

Crear directorio .ssh para que el nodo maestro pueda obtener el key al conectarse.

```bash
mkdir  .ssh
```

Se le otorgan los permisos al folder recien creado.

```bash
chmod  700  .ssh
```

Se mueve el texo del archivo id_rsa dentro del archivo "authorized_keys" en .ssh.

```bash
cat  id_rsa.pub >> ~/.ssh/authorized_keys
```

Si al correr el programa paralelizado da problemas la conexión, intentar hacer este paso anterior solo con este comando, el usuario del esclavo y el ip del esclavo

```bash
ssh-copy-id  slaveUser@172.18.64.131
```

**Para acceder al nodo esclavo desde el nodo maestro**

Se hace una conexión vía ssh

```bash
ssh  slaveUser@172.18.64.131
```

Ahora se puede controlar la máquina a la cual se conectó.

Para terminar la conexión

```bash
exit
```

Se deben configurar así cada uno de los nodos.

Editar los archivos hosts

```bash
sudo  nano  /etc/hosts
```

Se abrirá el archivo para editar y se deben agregar las direcciones ip del master y los nodos esclavos.

```txt
172.18.64.131 master master

172.18.30.251 node1 node1

172.18.67.123 node2 node2
```

Ahora se podrá conectar por ssh a partir del nombre guardado, pero con el usuario del nodo

```bash
ssh  slaveUser@node1
```

Si tienen el mismo usuario, no es necesario ponerlo

```bash
ssh  node2
```

6. Configuración del servidor de archivos NFS

En cada nodo del cluster, se necesita que todos los programas distribuidos por ejecutar tengan a disposición una carpeta compartida entre ellos. Esto será posible con la librería instalada anteriormente, nfs-kernel-server.

Se configurará el recurso compartido en el **nodo master**.

Se creará un directorio en el home del nodo maestro. Este debería quedar a la par del directorio con nombre del usuario en /home/clusterdir/.

```bash
cd && cd  ..
```
```bash
sudo  mkdir  clusterdir
```

Como root, se exportará el directorio para que se pueda acceder remotamente a través del archivo export. Se abrirá para editar el archivo.

```bash
sudo  nano  /etc/exports
```

Se debe agregar la ruta del directorio de la carpeta en el nodo maestro con las ips de los nodos esclavos.

```txt
/home/clusterdir 172.18.64.131(rw,no_subtree_check,async,fsid=0,no_root_squash,insecure)
```

Se debe reiniciar el servicio de nfs.

```bash
sudo  /etc/init.d/nfs-kernel-server  restart
```

Para verificar que se pueda acceder a la carpeta compartida desde el nodo esclavo se utiliza el comando showmount con el ip del nodo master.

```bash
showmount  -e  172.18.64.131
```

Se montará el recurso desde el cli para ser agregado a fstab. Así se hará el montado automático para que cada vez que arranque el nodo se cree en /home del nodo el directorio clusterdir igual que el nodo maestro. Ahí estarán los recursos compartidos del nodo maestro a través de nfs.

Ahora, se configurará el recurso compartido en el **nodo esclavo**.

Se creará el directorio al igual que en el nodo maestro.

```bash
sudo  mkdir  ../clusterdir
```

Se abre el archivo fstab para editarlo.

```bash
sudo  nano  /etc/fstab
```

Se modifica el archivo fstab agregando al final el ip del nodo maestro, la dirección del directorio del nodo maestro, la dirección del directorio del nodo esclavo.

```txt
172.18.30.251:/home/clusterdir /home/clusterdir nfs rw,sync,hard,intr 0 0
```

Se monta

```bash
sudo  mount  -a
```

Para verificar la conexión.

```bash
mount
```

Si se desea desmontar **(no hacer si no es necesario)**.

```bash
sudo  umount  -a
```

7. Configuración del entorno de desarrollo

El paquete de build-essentials tiene lo necesario para desarrollar y compilar, que ya se intenta instalar al inicio (usualmente ya viene en el SO).

8. Configuración para MPI

**Continuando dentro del directorio /home/clusterdir/.**

Se crea un archivo de compilación .mpi_hostfile en el **nodo maestro**.

```bash
sudo  nano  .mpi_hostfile_pso
```

Se edita y se le agrega.

```txt
#Nodo maestro
localhost slots=1

#Nodo esclavo1
node1 slots=1

#Nodo esclavo2
node2 slots=1
```

Cada slot es la cantidad de procesos que cada nodo puede ejecutar. Esto depende de la cantidad de cores del computador.

**Para este punto, ya se podría realizar una prueba.**

9. Compilación del código por paralelizar

El código por correr paralelamente debe estar dentro de la carpeta compartida junto a los archivos que necesite.

> Debe incluir la libreria mpi y sus comandos para correrlo paralelamente.

Se debe compilar con **mpicc**, el compilador propio de MPI para el lenguaje c.

```bash
mpicc  -o <executable_name> <program_code_file> -lm
```

Se hizo una carpeta para hacer una prueba del cluster: TestMPI. Esta incluye un código de prueba para ver todos los procesos que se pueden correr y de qué maquina.

Acceder a esta.

```bash
cd  PCMaster/TestMPI
```

Para compilar el test:

```bash
sudo  env  "PATH=$PATH"  "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"  mpicc  -o  hello_mpi  hello_mpi.c  -lm
```

> Se utiliza **sudo** por estar en un directorio con permisos privilegiados.

10. Ejecución del programa paralelizable con mpi

El comando para correrlo con OpenMPI sería como este.

```bash
mpirun  -np <number_of_processes> --hostfile <hostfile_name> ./<executable_name>
```

Por ejemplo, para correr el test:

```bash
mpirun  -np  3  --hostfile  ../.mpi_hostfile_pso  ./hello_mpi
```

11. Volver a correr el programa después de apagar las computadoras o si se pierde la conexión del mount nfs

Reiniciar el servicio de nfs en el **nodo master**.

```bash
sudo  /etc/init.d/nfs-kernel-server  restart
```

Verificar que el nodo master siga compartiendo la carpeta por nfs en el **nodo esclavo**.

```bash
showmount  -e  172.18.64.131
```

Volver a hacer el mount en el **nodo esclavo**.

```bash
sudo  mount  -a
```

Se recomienda salirse de la carpeta /home/clusterdir/ y volver a ingresar, si se encuentra en esta, para que los archivos carguen nuevamente.



### II. ServerMPI

1. Instalar la biblioteca de Desarrollo de OpenSSL

```bash
sudo apt install libssl-dev
```

2. Compilar el proyecto

Ir al folder ServerMPI con el Makefile.
```bash
cd /PCMASTER/ServerMPI/
```

Para compilar.
```bash
make
```

Para limpiar el proyecto.
```bash
make clean
```

3. Ejecutar el servidor

Ejecutar el proyecto localmente con MPI localmente.

```bash
make  run  NP=4
```

Ejecutar el proyecto en un entorno MPI distribuido usando un archivo de hosts.

```bash
make  run_mpi  NP=18
```



### III. Monitoreo de Tráfico de Red en Tiempo Real

1. Para monitorear el tráfico de red en tiempo real de una manera gráfica en la terminal.

Instalar el programa nload.
```bash
sudo apt install nload
```

Para ejecutarlo.
```bash
sudo nload
```

Si se desea monitorear una interfaz específica.
```bash
sudo nload eth0
```



### IV. Imagen Raspberry Pi

1. Para instalar un sistema operativo en el Raspberry Pi se necesitará el software de Raspberry Pi Imager.

Descargar de la página oficial de [Raspberry Pi](http://www.raspberrypi.com/software/).

O bien, se puede instalar por medio de una terminal.

```bash
sudo  apt  install  rpi-imager
```

2. Crear la imagen del sistema operativo

Se estará instalando el sistema operativo Ubuntu en la tarjeta.

Se debe buscar la aplicación del Imager y abrirla. Ahí se escogerá el sistema operativo Ubuntu Desktop (64-BIT). Se necesitará una tarjeta micro sd para flashear la imagen, esta debe ser escogida en el programa.

Con esto se puede comenzar a escribir en la tarjeta.

Al finalizar de crear la imagen en la tarjeta, se ejecta del computador y se ingresa a la Raspberry Pi.

3. Bootear el sistema operativo

Con la tarjeta ya ingresada en su slot correspondiente en la Raspberry Pi, se procede a conectarla a la corriente. Esto hará que haga el boot desde la tarjeta micro sd.

Seguir las indicaciones para configurar el sistema operativo. 

> Para el proyecto se estará utilizando Ubuntu.

4. Actualizar el sistema operativo

Como recomendación actualizar el sistema operativo recién instalado si hay oportunidad.

5. Utilizando el Raspberry Pi

Si se conecta por HDMI a un monitor, este funcionará como cualquier otro computador manejado por un teclado y un mouse. Aún así, se le configurará una conexión para accederlo por ssh.

6. Configuración de red estática

Ir a la parte de la red cableada en la configuración de red.

En la parte de IPv4:

Asignar el método manual.

**Asignar una Dirección:**

- Dirección: 192.168.50.180
- Máscara de red: 255.255.255.0
- Puerta de enlace: 192.168.50.1

**Configurar el DNS:**

Desmarcar el automático.

- DNS: 192.168.50.1

> Desconectarse y volverse a conectar para que los cambios ocurran.

7. Descarga de dependencias para la configuración del acceso

Instalar dependencias de ssh.

```bash
sudo  apt-get  install  ssh
```

8. Configuración del computador que se conectará al Raspberry Pi

Para que un computador pueda ser capaz de ingresar y manejar el Raspberry Pi se necesita que este pueda tener acceso al ssh.

Se crea una llave para el ssh

```bash
ssh-keygen -t  rsa
```

No asignar file.
Dejar empty el passphrase.

Se crea el key de forma aleatoria.

Se procede a copiar la llave que se acaba de crear en el computador, por medio del protocolo SCP.

```bash
scp  .ssh/id_rsa.pub  rpiUser@192.168.50.180:/home/rpiUser
```

Ahora, en el Raspberry.

Crear directorio ".ssh" para que el computador pueda obtener el key al conectarse.

```bash
mkdir  .ssh
```

Se le otorgan los permisos al folder.

```bash
chmod  700  .ssh
```

Se mueve el archivo dentro de .ssh como "authorized_keys".

```bash
cat  id_rsa.pub >> ~/.ssh/authorized_keys
```

Si este proceso da algun problema copiar el key con este comando.

```bash
ssh-copy-id  rpiUser@192.168.50.180
```

9. Acceder al Raspberry Pi desde el computador

Se hace una conexión vía ssh.

```bash
ssh  rpiUser@192.168.50.180
```

Ahora se puede controlar la Raspberry Pi a la cual se conectó.

Para terminar la conexión.

```bash
exit
```

Para facilitar la conexión, se puede editar el archivo *hosts* del computador.

```bash
sudo  nano  /etc/hosts
```

Se abrirá el archivo para editar y se debe agregar la dirección ip del Raspberry Pi.

```txt
192.168.50.180 rpi rpi
```

Ahora se podrá conectar por ssh a partir del nombre guardado, pero con el usuario del nodo.

```bash
ssh  rpiUser@rpi
```

Si tienen el mismo usuario, no es necesario ponerlo al conectarse.

```bash
ssh  rpi
```

Para terminar la conexión, se mantiene el mismo comando.

```bash
exit
```



### V. Driver

Para la utilización de este driver es esencial contar al menos con el lcd_driver.c.

1. Compilación del Driver

En el terminal, navegar al directorio donde se encuentra el código fuente del driver (lcd_driver.c) y el Makefile. Ejecutar el siguiente comando.

```bash
make
```

O se puede realizar la compilación sin la necesidad de un archivo make, ejecutando los siguientes comandos.

```bash
make  -C  /lib/modules/$(uname -r)/build  M=$(pwd) modules

make  -C  /lib/modules/$(uname -r)/build  M=$(pwd) clean
```

Estos comandos compilarán el driver y generarán un archivo .ko (módulo del kernel), en este caso, lcd_driver.ko.

2. Instalación del driver (cargar el módulo):

Una vez compilado el módulo, cargarlo al kernel de Linux usando insmod.

```bash
sudo  insmod  lcd_driver.ko
```

Para verificar que el módulo se haya cargado correctamente.

```bash
lsmod | grep  lcd_driver
```

3. Verificar que el Driver Está Funcionando

Se puede comprobar que el dispositivo ha sido creado correctamente y que el módulo está activo verificando la creación de laruta definida para el dispositivo, /dev/lcd.

```bash
ls  /dev/lcd
```

4. Eliminación del Driver

Si se necesita quitar el driver.

```bash
sudo  rmmod  lcd_driver
```

Este comando elimina el módulo del kernel.



### VI. Biblioteca

1. Instalar la biblioteca

Para utilizar la biblioteca en cualquier programa, es necesario copiar el archivo "liblcd_buzzer" a un directorio estándar como /usr/local/lib.

```bash
sudo  cp  liblcd_buzzer.a  /usr/local/lib/
```

Asegurarse que el compilador pueda encontrar los encabezados de la biblioteca, por lo que se debe copiar el archivo .h asociado a la biblioteca en /usr/local/include.

```bash
sudo  cp  lcd_buzzer.h  /usr/local/include/
```

2. Uso de la biblioteca

Una vez instalado el driver y la biblioteca, es posible incluir la biblioteca a los programas de C.

Para utilizar la biblioteca incluir el archivo .h al programa.

```c
#include "lcd_driver_lib.h"
```

3. Vincular la biblioteca al compilar

Cuando se compile el programa que utiliza la biblioteca, indicar al compilador que enlace la biblioteca estática .a generada utilizando los siguientes parametros.

```bash
gcc  main.c  -L/usr/local/lib  -llcd_driver  -o  main_program
```

`-L/usr/local/lib`: Especifica la ruta donde está la biblioteca.

`-llcd_driver`: Le indica al compilador que enlace la biblioteca liblcd_driver.a.

`-o main_program`: Especifica el nombre del ejecutable final.

4. Funciones

`driver_write(message)`: Recibe como parametro un mensaje de máximo 32 caractéres, el cual mostrará en la pantalla LCD.

`play_melody()`: Reproduce una melodía por el buzzer (Fur Elise).

`beep_buzzer(n)`: Hace sonar el buzzer n veces.



### VII. Cliente

1. Instalar la biblioteca de Desarrollo de OpenSSL

```bash
sudo apt install libssl-dev
```

2. Compilar el cliente

Ir al folder Client con el Makefile.

```bash
cd  /Raspberry/Client/
```

Para compilar.

```bash
make
```

Para limpiar el proyecto.

```bash
make  clean
```

3. Ejecutar el cliente

```bash
make  run
```
